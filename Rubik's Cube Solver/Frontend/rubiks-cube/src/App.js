import { useEffect, useRef, useState } from 'react';
import * as THREE from 'three';
import { RoundedBoxGeometry } from 'three/addons/geometries/RoundedBoxGeometry.js';

const FACE_LETTERS = ['R', 'L', 'U', 'D', 'F', 'B'];

const CENTER_COLOR = {
  R: '#B5651D',
  L: '#A83232',
  U: '#C9A227',
  D: '#D8D5C9',
  F: '#2F5D3A',
  B: '#25476B',
};

const COLORS = ['#C9A227', '#D8D5C9', '#B5651D', '#2F5D3A', '#A83232', '#25476B'];
const UNPAINTED = '#2a2a2a';
const INPUT_FACE_ORDER = ['U', 'D', 'F', 'R', 'B', 'L']; // Yellow → White → Green → Orange → Blue → Red
const BACKEND_FACE_ORDER = ['U', 'D', 'L', 'R', 'F', 'B'];
const COLOR_TO_INTEGER = {
  '#C9A227': 0, // Yellow
  '#D8D5C9': 1, // White
  '#A83232': 2, // Red
  '#B5651D': 3, // Orange
  '#2F5D3A': 4, // Green
  '#25476B': 5, // Blue
};

const MOVE_FACES = ['R', 'L', 'U', 'D', 'F', 'B'];
const MOVE_BUTTONS = MOVE_FACES.flatMap((face) => [face, `${face}'`, `${face}2`]);
const TURN_CONFIG = {
  R: { axis: 'x', layer: 1, direction: -1 },
  L: { axis: 'x', layer: -1, direction: 1 },
  U: { axis: 'y', layer: 1, direction: -1 },
  D: { axis: 'y', layer: -1, direction: 1 },
  F: { axis: 'z', layer: 1, direction: -1 },
  B: { axis: 'z', layer: -1, direction: 1 },
};
const SOLVE_VIEW_ROTATION = [Math.PI / 6, -Math.PI / 6, 0];

// Which face sits up/down/left/right of each face when it's the one facing the camera
const NEIGHBORS = {
  F: { up: 'U', down: 'D', left: 'L', right: 'R' },
  U: { up: 'B', down: 'F', left: 'L', right: 'R' },
  D: { up: 'F', down: 'B', left: 'L', right: 'R' },
  L: { up: 'U', down: 'D', left: 'B', right: 'F' },
  R: { up: 'U', down: 'D', left: 'F', right: 'B' },
  B: { up: 'U', down: 'D', left: 'R', right: 'L' },
};

// Euler rotation (in radians) that brings each face to point straight at the camera
const FACE_ROTATION = {
  F: [0, 0, 0],
  B: [0, Math.PI, 0],
  U: [Math.PI / 2, 0, 0],
  D: [-Math.PI / 2, 0, 0],
  R: [0, -Math.PI / 2, 0],
  L: [0, Math.PI / 2, 0],
};

function makeInitialCube() {
  const cube = {};
  FACE_LETTERS.forEach((face) => {
    const stickers = Array(9).fill(UNPAINTED);
    stickers[4] = CENTER_COLOR[face];
    cube[face] = stickers;
  });
  return cube;
}

function countColors(cube) {
  const counts = {};
  COLORS.forEach((c) => (counts[c] = 0));
  FACE_LETTERS.forEach((face) => {
    cube[face].forEach((c) => {
      if (counts[c] !== undefined) counts[c] += 1;
    });
  });
  return counts;
}

// Each face is already stored from its displayed top-left to bottom-right.
export function serializeCube(cube) {
  return BACKEND_FACE_ORDER.flatMap((face) =>
    cube[face].map((color) => COLOR_TO_INTEGER[color])
  );
}

function stickerCoordinate(face, index) {
  const row = Math.floor(index / 3);
  const col = index % 3;
  if (face === 'F') return { normal: [0, 0, 1], position: [col - 1, 1 - row, 1] };
  if (face === 'B') return { normal: [0, 0, -1], position: [1 - col, 1 - row, -1] };
  if (face === 'U') return { normal: [0, 1, 0], position: [col - 1, 1, row - 1] };
  if (face === 'D') return { normal: [0, -1, 0], position: [col - 1, -1, 1 - row] };
  if (face === 'R') return { normal: [1, 0, 0], position: [1, 1 - row, 1 - col] };
  return { normal: [-1, 0, 0], position: [-1, 1 - row, col - 1] }; // L
}

function coordinateSticker(normal, position) {
  const [x, y, z] = position;
  if (normal[2] === 1) return { face: 'F', index: (1 - y) * 3 + x + 1 };
  if (normal[2] === -1) return { face: 'B', index: (1 - y) * 3 + 1 - x };
  if (normal[1] === 1) return { face: 'U', index: (z + 1) * 3 + x + 1 };
  if (normal[1] === -1) return { face: 'D', index: (1 - z) * 3 + x + 1 };
  if (normal[0] === 1) return { face: 'R', index: (1 - y) * 3 + 1 - z };
  return { face: 'L', index: (1 - y) * 3 + z + 1 };
}

function rotateVector([x, y, z], axis, direction) {
  if (axis === 'x') return direction === 1 ? [x, -z, y] : [x, z, -y];
  if (axis === 'y') return direction === 1 ? [z, y, -x] : [-z, y, x];
  return direction === 1 ? [-y, x, z] : [y, -x, z];
}

function applyQuarterTurn(cube, face, inverse = false) {
  const { axis, layer, direction } = TURN_CONFIG[face];
  const turnDirection = inverse ? -direction : direction;
  const axisIndex = axis === 'x' ? 0 : axis === 'y' ? 1 : 2;
  const next = Object.fromEntries(FACE_LETTERS.map((cubeFace) => [cubeFace, [...cube[cubeFace]]]));

  FACE_LETTERS.forEach((sourceFace) => {
    cube[sourceFace].forEach((color, index) => {
      const { normal, position } = stickerCoordinate(sourceFace, index);
      if (position[axisIndex] !== layer) return;
      const destination = coordinateSticker(
        rotateVector(normal, axis, turnDirection),
        rotateVector(position, axis, turnDirection)
      );
      next[destination.face][destination.index] = color;
    });
  });
  return next;
}

function applyMove(cube, move) {
  const face = move[0];
  const inverse = move.includes("'");
  const turns = move.includes('2') ? 2 : 1;
  let next = cube;
  for (let turn = 0; turn < turns; turn += 1) next = applyQuarterTurn(next, face, inverse);
  return next;
}

function App({ onCubeComplete }) {
  const mountRef = useRef(null);
  const [cube, setCube] = useState(makeSolvedCube());
  const cubeRef = useRef(cube);
  const [activePopup, setActivePopup] = useState(null);
  const [currentFace, setCurrentFace] = useState('F');
  const currentFaceRef = useRef('F');
  const [mode, setMode] = useState('landing'); // 'landing' | 'input'
  const [visibleWordsCount, setVisibleWordsCount] = useState(0);
  const [showEnterButton, setShowEnterButton] = useState(false);
  const [isCubeComplete, setIsCubeComplete] = useState(false);
  const [isSolveMode, setIsSolveMode] = useState(false);
  const [isSolutionFinished, setIsSolutionFinished] = useState(false);
  const [showNewScrambleButton, setShowNewScrambleButton] = useState(false);
  const [sceneKey, setSceneKey] = useState(0);
  const isSolutionFinishedRef = useRef(false);
  const [solutionMoves, setSolutionMoves] = useState([]);
  const [moveInputError, setMoveInputError] = useState('');
  const [runMode, setRunMode] = useState(null);
  const [moveIndex, setMoveIndex] = useState(0);
  const [isMoveBusy, setIsMoveBusy] = useState(false);
  const [isAutoRunning, setIsAutoRunning] = useState(false);
  const [isPaused, setIsPaused] = useState(false);
  const modeRef = useRef('landing');
  const isSolveModeRef = useRef(false);

  const stickerMaterialsRef = useRef({});
  const cubeMeshRef = useRef(null);
  const cubeArrayRef = useRef(null);
  const enqueueMoveRef = useRef(null);
  const clearMoveQueueRef = useRef(null);
  const pauseRef = useRef(false);
  const pauseAfterMoveRef = useRef(false);
  const tweenRef = useRef(null); // { from: Quaternion, to: Quaternion, start: time, duration }
  const activeMoveRef = useRef(null);

  useEffect(() => {
    if (activeMoveRef.current) {
      activeMoveRef.current.scrollIntoView({ behavior: 'smooth', inline: 'center', block: 'nearest' });
    }
  }, [moveIndex]);

  useEffect(() => {
    cubeRef.current = cube;
  }, [cube]);

  useEffect(() => {
    if (isSolveMode && moveIndex === solutionMoves.length) {
      isSolutionFinishedRef.current = true;
      setIsSolutionFinished(true);
    } else {
      isSolutionFinishedRef.current = false;
      setIsSolutionFinished(false);
    }
  }, [moveIndex, solutionMoves.length, isSolveMode]);

  useEffect(() => {
    if (isSolutionFinished) {
      const timer = setTimeout(() => setShowNewScrambleButton(true), 1000);
      return () => clearTimeout(timer);
    } else {
      setShowNewScrambleButton(false);
    }
  }, [isSolutionFinished]);

  const solverModuleRef = useRef(null);

  function makeSolvedCube() {
  const cube = {};
  FACE_LETTERS.forEach((face) => {
    cube[face] = Array(9).fill(CENTER_COLOR[face]);
  });
  return cube;
}

  function applyColor(hex) {
    if (!activePopup) return;
    const { face, stickerIdx } = activePopup;
    const faceWasIncomplete = cubeRef.current[face].some(
      (color, index) => index !== 4 && color === UNPAINTED
    );
    const cubeWasIncomplete = FACE_LETTERS.some((cubeFace) =>
      cubeRef.current[cubeFace].some((color) => color === UNPAINTED)
    );
    const updated = { ...cubeRef.current };
    updated[face] = [...updated[face]];
    updated[face][stickerIdx] = hex;
    cubeRef.current = updated;
    setCube(updated);
    stickerMaterialsRef.current[face][stickerIdx].color.set(hex);
    setActivePopup(null);

    const faceIsComplete = updated[face].every(
      (color, index) => index === 4 || color !== UNPAINTED
    );
    const faceOrderIndex = INPUT_FACE_ORDER.indexOf(face);
    if (faceWasIncomplete && faceIsComplete && faceOrderIndex < INPUT_FACE_ORDER.length - 1) {
      focusFace(INPUT_FACE_ORDER[faceOrderIndex + 1]);
    }

    const cubeIsComplete = FACE_LETTERS.every((inputFace) =>
      updated[inputFace].every((color) => color !== UNPAINTED)
    );
    if (cubeWasIncomplete && cubeIsComplete) {
      cubeArrayRef.current = serializeCube(updated);
      setIsCubeComplete(true);
      onCubeComplete?.(cubeArrayRef.current);
      console.info('Completed cube array:', cubeArrayRef.current.join(' '));
    }
  }

  function startInput() {
  if (!solverModuleRef.current) {
    if (!window.CubeSolverModule) {
      const script = document.createElement('script');
      script.src = process.env.PUBLIC_URL + '/wasm/solver.js';
      script.onload = () => {
        window.CubeSolverModule({ locateFile: (path) => process.env.PUBLIC_URL + '/wasm/' + path }).then((Module) => {
          solverModuleRef.current = Module;
        });
      };
      document.body.appendChild(script);
    } else {
      window.CubeSolverModule({ locateFile: (path) => process.env.PUBLIC_URL + '/wasm/' + path }).then((Module) => {
        solverModuleRef.current = Module;
      });
    }
  }
  // The Three.js event handler and render loop read this ref, so update it
  // immediately instead of waiting for React state to re-render.
  modeRef.current = 'input';
  isSolveModeRef.current = false;
  cubeArrayRef.current = null;
  setIsCubeComplete(false);
  setIsSolveMode(false);
  setIsSolutionFinished(false);
  setShowNewScrambleButton(false);
  isSolutionFinishedRef.current = false;
  setVisibleWordsCount(0);
  setSceneKey((prev) => prev + 1);
  setShowEnterButton(false);
  setSolutionMoves([]);
  setMoveInputError('');
  setRunMode(null);
  setMoveIndex(0);
  setIsMoveBusy(false);
  setIsAutoRunning(false);
  setIsPaused(false);
  pauseRef.current = false;
  pauseAfterMoveRef.current = false;
  const [rx, ry, rz] = FACE_ROTATION.U;
  const targetQuat = new THREE.Quaternion().setFromEuler(new THREE.Euler(rx, ry, rz));
  tweenRef.current = {
    from: cubeMeshRef.current.quaternion.clone(),
    to: targetQuat,
    start: performance.now(),
    duration: 800,
  };
  currentFaceRef.current = 'U';
  setCurrentFace('U');

  const blank = makeInitialCube();
  cubeRef.current = blank;
  setCube(blank);
  FACE_LETTERS.forEach((face) => {
    blank[face].forEach((color, stickerIdx) => {
      stickerMaterialsRef.current[face][stickerIdx].color.set(color);
    });
  });

  setMode('input');
  }

  function focusFace(nextFace) {
    const [rx, ry, rz] = FACE_ROTATION[nextFace];
    const targetQuat = new THREE.Quaternion().setFromEuler(new THREE.Euler(rx, ry, rz));
    tweenRef.current = {
      from: cubeMeshRef.current.quaternion.clone(),
      to: targetQuat,
      start: performance.now(),
      duration: 700,
    };
    currentFaceRef.current = nextFace;
    setCurrentFace(nextFace);
    setActivePopup(null);
  }

  function navigate(direction) {
    focusFace(NEIGHBORS[currentFaceRef.current][direction]);
  }

async function startSolve() {
  if (!cubeArrayRef.current) {
    setMoveInputError('Cube data is missing. Please enter the cube again.');
    return;
  }

  let Module = solverModuleRef.current;

  if (!Module) {
    setMoveInputError('Loading solver...');
    try {
      Module = await window.CubeSolverModule({ locateFile: (path) => '/wasm/' + path });
      solverModuleRef.current = Module;
    } catch (error) {
      console.error('Could not load solver:', error);
      setMoveInputError('Could not load the solver. Check the browser console.');
      return;
    }
  }
  setMoveInputError('Solving cube...');
  const inputVec = new Module.VectorInt();
  let resultVec;
  let moves = [];
  try {
    cubeArrayRef.current.forEach((v) => inputVec.push_back(v));
    resultVec = Module.solveCube(inputVec);
    for (let i = 0; i < resultVec.size(); i++) moves.push(resultVec.get(i));
  } catch (error) {
    console.error('Cube solver failed:', error);
    setMoveInputError('The solver could not process this cube. Please check the sticker colors and try again.');
    return;
  } finally {
    inputVec.delete();
    resultVec?.delete();
  }

  if (moves[0]?.startsWith('ERROR:')) {
    setMoveInputError(moves[0].slice('ERROR: '.length));
    return;
  }

  if (moves[0]?.startsWith('T')) {
    setSolutionMoves(moves);
    isSolveModeRef.current = true;
    setIsSolveMode(true);
    setRunMode(null);
    setMoveIndex(1);
    setIsPaused(false);
    pauseRef.current = false;
    pauseAfterMoveRef.current = false;
    focusSolveView();
    return;
  }

  setSolutionMoves(moves);
  setMoveInputError(moves.length ? `Solution found: ${moves.length} moves.` : 'Cube is already solved.');
  isSolveModeRef.current = true;
  setIsSolveMode(true);
  setRunMode(null);
  setMoveIndex(0);
  setIsPaused(false);
  pauseRef.current = false;
  pauseAfterMoveRef.current = false;
  focusSolveView();
}

  function inverseMove(move) {
    if (move.endsWith('2')) return move;
    return move.endsWith("'") ? move.slice(0, -1) : `${move}'`;
  }

  function runManualMove(direction) {
    if (isMoveBusy) return;
    const targetIndex = direction === 'next' ? moveIndex : moveIndex - 1;
    if (targetIndex < 0 || targetIndex >= solutionMoves.length) return;
    const move = direction === 'next' ? solutionMoves[targetIndex] : inverseMove(solutionMoves[targetIndex]);
    pauseRef.current = false;
    setIsPaused(false);
    setIsMoveBusy(true);
    enqueueMoveRef.current?.(move, {
      duration: 500,
      onComplete: () => {
        setMoveIndex(direction === 'next' ? targetIndex + 1 : targetIndex);
        setIsMoveBusy(false);
      },
    });
  }

function queueRemainingMoves(duration) {
  const startIndex = moveIndex;
  solutionMoves.slice(startIndex).forEach((move, offset, remainingMoves) => {
    enqueueMoveRef.current?.(move, {
      duration,
      onComplete: () => {
        setMoveIndex(startIndex + offset + 1);
        if (offset === remainingMoves.length - 1) {
          setIsAutoRunning(false);
          setIsMoveBusy(false);
        }
      },
    });
  });
}

function runAutomaticMoves(duration) {
  if (isAutoRunning || isMoveBusy || moveIndex >= solutionMoves.length) return;
  setRunMode(`auto-${duration}`);
  setIsAutoRunning(true);
  setIsPaused(false);
  pauseRef.current = false;
  pauseAfterMoveRef.current = false;
  setIsMoveBusy(true);
  queueRemainingMoves(duration);
}

// New: lets the user pick a different speed (or drop to manual) while paused mid-solve
function changeSpeed(duration) {
  if (!isAutoRunning) { runAutomaticMoves(duration); return; }
  if (!isPaused || isMoveBusy || moveIndex >= solutionMoves.length) return;
  clearMoveQueueRef.current?.();       // drop the stale, old-duration queue
  setRunMode(`auto-${duration}`);
  setIsPaused(false);
  pauseRef.current = false;
  pauseAfterMoveRef.current = false;
  setIsMoveBusy(true);
  queueRemainingMoves(duration);       // re-queue what's left at the new speed
}

function switchToManual() {
  if (isAutoRunning) {
    if (!isPaused || isMoveBusy) return;
    clearMoveQueueRef.current?.();
    setIsAutoRunning(false);
  }
  setRunMode('manual');
}

  function togglePlayback() {
    if (!isAutoRunning) return;
    if (isPaused) {
      pauseAfterMoveRef.current = false;
      pauseRef.current = false;
      setIsPaused(false);
      return;
    }
    setIsPaused(true);
    if (isMoveBusy) {
      // Finish the current turn cleanly, then stop on a stable cube state.
      pauseAfterMoveRef.current = true;
    } else {
      pauseRef.current = true;
    }
  }

  function seekToCompletedMove(targetIndex) {
    if (!isPaused || isMoveBusy || targetIndex >= moveIndex) return;
    const rewindMoves = solutionMoves.slice(targetIndex, moveIndex).reverse().map(inverseMove);
    if (rewindMoves.length === 0) return;
    clearMoveQueueRef.current?.();
    setIsAutoRunning(false);
    setRunMode('manual');
    setIsMoveBusy(true);
    setIsPaused(false);
    pauseRef.current = false;
    pauseAfterMoveRef.current = false;
    rewindMoves.forEach((move, index) => {
      enqueueMoveRef.current?.(move, {
        duration: 500,
        onComplete: () => {
          if (index === rewindMoves.length - 1) {
            setMoveIndex(targetIndex);
            setIsMoveBusy(false);
            setIsPaused(true);
            pauseRef.current = true;
          }
        },
      });
    });
  }

  function focusSolveView() {
    const [rx, ry, rz] = SOLVE_VIEW_ROTATION;
    const targetQuat = new THREE.Quaternion().setFromEuler(new THREE.Euler(rx, ry, rz));
    tweenRef.current = {
      from: cubeMeshRef.current.quaternion.clone(),
      to: targetQuat,
      start: performance.now(),
      duration: 700,
    };
    currentFaceRef.current = 'F';
    setCurrentFace('F');
    setActivePopup(null);
  }

  useEffect(() => {
    if (modeRef.current === 'landing') {
      const blank = makeSolvedCube();
      cubeRef.current = blank;
      setCube(blank);
    }

    const mount = mountRef.current;

    const scene = new THREE.Scene();
    const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);
    const renderer = new THREE.WebGLRenderer({ antialias: true });
    renderer.setSize(window.innerWidth, window.innerHeight);
    renderer.setClearColor(0x1a1a1a);
    mount.appendChild(renderer.domElement);

    const handleResize = () => {
      camera.aspect = window.innerWidth / window.innerHeight;
      camera.updateProjectionMatrix();
      renderer.setSize(window.innerWidth, window.innerHeight);
    };
    window.addEventListener('resize', handleResize);

    const cubeGroup = new THREE.Group();
    const cubieGeometry = new RoundedBoxGeometry(0.98, 0.98, 0.98, 4, 0.08);
    const stickerShape = new THREE.Shape();
    const stickerSize = 0.78;
    const stickerRadius = 0.07;
    const halfSticker = stickerSize / 2;
    stickerShape.moveTo(-halfSticker + stickerRadius, -halfSticker);
    stickerShape.lineTo(halfSticker - stickerRadius, -halfSticker);
    stickerShape.quadraticCurveTo(halfSticker, -halfSticker, halfSticker, -halfSticker + stickerRadius);
    stickerShape.lineTo(halfSticker, halfSticker - stickerRadius);
    stickerShape.quadraticCurveTo(halfSticker, halfSticker, halfSticker - stickerRadius, halfSticker);
    stickerShape.lineTo(-halfSticker + stickerRadius, halfSticker);
    stickerShape.quadraticCurveTo(-halfSticker, halfSticker, -halfSticker, halfSticker - stickerRadius);
    stickerShape.lineTo(-halfSticker, -halfSticker + stickerRadius);
    stickerShape.quadraticCurveTo(-halfSticker, -halfSticker, -halfSticker + stickerRadius, -halfSticker);
    const stickerGeometry = new THREE.ShapeGeometry(stickerShape);
    const plasticMaterial = new THREE.MeshBasicMaterial({ color: 0x101010 });
    const stickerPlacement = {
      R: { position: [0.501, 0, 0], rotation: [0, Math.PI / 2, 0] },
      L: { position: [-0.501, 0, 0], rotation: [0, -Math.PI / 2, 0] },
      U: { position: [0, 0.501, 0], rotation: [-Math.PI / 2, 0, 0] },
      D: { position: [0, -0.501, 0], rotation: [Math.PI / 2, 0, 0] },
      F: { position: [0, 0, 0.501], rotation: [0, 0, 0] },
      B: { position: [0, 0, -0.501], rotation: [0, Math.PI, 0] },
    };
    FACE_LETTERS.forEach((face) => {
      stickerMaterialsRef.current[face] = Array(9);
    });

    // A real cube is 27 small cubies with slim gaps, rather than one solid box.
    for (let x = 0; x < 3; x++) {
      for (let y = 0; y < 3; y++) {
        for (let z = 0; z < 3; z++) {
          const cellForFace = {
            F: (2 - y) * 3 + x,
            B: (2 - y) * 3 + (2 - x),
            U: z * 3 + x,
            D: (2 - z) * 3 + x,
            R: (2 - y) * 3 + (2 - z),
            L: (2 - y) * 3 + z,
          };
          const cubie = new THREE.Mesh(cubieGeometry, plasticMaterial);
          FACE_LETTERS.forEach((face) => {
            const isOuterFace =
              (face === 'R' && x === 2) || (face === 'L' && x === 0) ||
              (face === 'U' && y === 2) || (face === 'D' && y === 0) ||
              (face === 'F' && z === 2) || (face === 'B' && z === 0);
            if (!isOuterFace) return;

            const stickerIdx = cellForFace[face];
            const material = new THREE.MeshBasicMaterial({ color: cubeRef.current[face][stickerIdx] });
            stickerMaterialsRef.current[face][stickerIdx] = material;
            const sticker = new THREE.Mesh(stickerGeometry, material);
            const { position, rotation } = stickerPlacement[face];
            sticker.position.set(...position);
            sticker.rotation.set(...rotation);
            sticker.userData = { face, stickerIdx };
            cubie.add(sticker);
          });
          cubie.position.set((x - 1) * 1.02, (y - 1) * 1.02, (z - 1) * 1.02);
          cubie.userData.gridPosition = [x - 1, y - 1, z - 1];
          cubeGroup.add(cubie);
        }
      }
    }

    cubeMeshRef.current = cubeGroup;
    scene.add(cubeGroup);
    camera.position.set(0, 0, 7);
    camera.lookAt(0, 0, 0);

    const moveQueue = [];
    let activeTurn = null;
    const axisIndex = (axis) => (axis === 'x' ? 0 : axis === 'y' ? 1 : 2);

    enqueueMoveRef.current = (move, options = {}) => {
      if (MOVE_BUTTONS.includes(move)) moveQueue.push({ move, ...options });
    };
    clearMoveQueueRef.current = () => {
      moveQueue.length = 0;
    };

    function beginNextMove() {
      // Do not rotate a layer while the whole cube is still moving into its view.
      if (activeTurn || tweenRef.current || pauseRef.current || moveQueue.length === 0) return;
      if (pauseAfterMoveRef.current) {
        pauseAfterMoveRef.current = false;
        pauseRef.current = true;
        setIsMoveBusy(false);
        return;
      }
      const queuedMove = moveQueue.shift();
      const { move } = queuedMove;
      const face = move[0];
      const { axis, layer, direction } = TURN_CONFIG[face];
      const turnDirection = move.includes("'") ? -direction : direction;
      const turns = move.includes('2') ? 2 : 1;
      const selectedCubies = cubeGroup.children.filter(
        (child) => child.userData.gridPosition?.[axisIndex(axis)] === layer
      );
      const turnGroup = new THREE.Group();
      cubeGroup.add(turnGroup);
      selectedCubies.forEach((cubie) => turnGroup.attach(cubie));
      activeTurn = {
        move,
        axis,
        turnDirection,
        turns,
        selectedCubies,
        turnGroup,
        start: performance.now(),
        elapsed: 0,
        lastFrameTime: performance.now(),
        duration: queuedMove.duration ?? 500,
        angle: turnDirection * (move.includes('2') ? Math.PI : Math.PI / 2),
        onComplete: queuedMove.onComplete,
      };
      queuedMove.onStart?.();
    }

    const raycaster = new THREE.Raycaster();
    const pointer = new THREE.Vector2();

    function onClick(event) {
      pointer.x = (event.clientX / window.innerWidth) * 2 - 1;
      pointer.y = -(event.clientY / window.innerHeight) * 2 + 1;
      raycaster.setFromCamera(pointer, camera);
      const hits = raycaster.intersectObjects(cubeGroup.children, true);
      if (hits.length === 0) return;

      const hit = hits[0];
      const { face, stickerIdx } = hit.object.userData;
      if (!face) return; // Clicked the recessed black plastic, not a sticker.
      if (modeRef.current !== 'input') return;
      if (isSolveModeRef.current) return;
      if (face !== currentFaceRef.current) return; // ignore clicks on faces not facing the camera

      if (stickerIdx === 4) return;

      setActivePopup({ face, stickerIdx, x: event.clientX, y: event.clientY });
    }
    renderer.domElement.addEventListener('click', onClick);

    let animationFrameId;
    const animate = () => {
      animationFrameId = requestAnimationFrame(animate);

      const tween = tweenRef.current;
      if (tween) {
        const t = Math.min(1, (performance.now() - tween.start) / tween.duration);
        cubeGroup.quaternion.slerpQuaternions(tween.from, tween.to, t);
        if (t >= 1) tweenRef.current = null;
      } else if (modeRef.current === 'landing' || isSolutionFinishedRef.current) {
          cubeGroup.rotation.x += 0.006;
          cubeGroup.rotation.y += 0.008;
        }

      if (activeTurn) {
        const now = performance.now();
        if (!pauseRef.current) {
          activeTurn.elapsed += now - activeTurn.lastFrameTime;
        }
        activeTurn.lastFrameTime = now;
        const progress = Math.min(1, activeTurn.elapsed / activeTurn.duration);
        activeTurn.turnGroup.rotation[activeTurn.axis] = activeTurn.angle * progress;
        if (progress === 1) {
          activeTurn.turnGroup.updateMatrixWorld(true);
          activeTurn.selectedCubies.forEach((cubie) => {
            cubeGroup.attach(cubie);
            let gridPosition = cubie.userData.gridPosition;
            for (let turn = 0; turn < activeTurn.turns; turn += 1) {
              gridPosition = rotateVector(gridPosition, activeTurn.axis, activeTurn.turnDirection);
            }
            cubie.userData.gridPosition = gridPosition;
            // Snap away floating-point drift after parenting the completed turn.
            cubie.position.set(...gridPosition.map((coordinate) => coordinate * 1.02));
          });
          cubeGroup.remove(activeTurn.turnGroup);
          const nextCube = applyMove(cubeRef.current, activeTurn.move);
          cubeRef.current = nextCube;
          setCube(nextCube);
          activeTurn.onComplete?.();
          if (pauseAfterMoveRef.current) {
            pauseAfterMoveRef.current = false;
            pauseRef.current = true;
            setIsMoveBusy(false);
          }
          if (pauseRef.current) setIsMoveBusy(false);
          activeTurn = null;
        }
      }

      if (!activeTurn) {
        beginNextMove();
      }

      renderer.render(scene, camera);
    };

    if (modeRef.current === 'landing') {
      const scramble = ['R', "U'", 'F2', "R'", 'D', "U'"];
      scramble.forEach((move) => enqueueMoveRef.current?.(move, { duration: 1 }));

      const solve = ['U', "D'", 'R', 'F2', 'U', "R'"];
      solve.forEach((move, i) => {
        enqueueMoveRef.current?.(move, {
          duration: 800,
          onStart: () => {
            if (i === 3) setVisibleWordsCount(1);
            if (i === 4) setVisibleWordsCount(2);
            if (i === 5) setVisibleWordsCount(3);
          },
          onComplete: () => {
            if (i === 5) setTimeout(() => setShowEnterButton(true), 1000);
          }
        });
      });
    }

    animate();

    return () => {
      window.removeEventListener('resize', handleResize);
      renderer.domElement.removeEventListener('click', onClick);
      cancelAnimationFrame(animationFrameId);
      enqueueMoveRef.current = null;
      clearMoveQueueRef.current = null;
      if (mount) mount.removeChild(renderer.domElement);
      renderer.dispose();
    };
  }, [sceneKey]);

  const counts = activePopup ? countColors(cube) : {};
  const neighbors = NEIGHBORS[currentFace];

  const dotStyle = (bg) => ({
    width: '18px',
    height: '18px',
    borderRadius: '4px',
    background: bg,
    border: 'none',
    cursor: 'pointer',
  });

  return (
    <div>
      <style>{`
        @keyframes slideUpFade {
          from { opacity: 0; transform: translateY(20px); }
          to { opacity: 1; transform: translateY(0); }
        }
      `}</style>
      <div ref={mountRef} />

      {mode === 'landing' && (
        <>
          <div className="responsive-title" style={{
            position: 'fixed',
            top: '15%',
            left: '50%',
            transform: 'translateX(-50%)'
          }}>
            {["Rubik's", "Cube", "Solver"].map((word, index) => (
              <span key={index} style={{ 
                opacity: 0, 
                animation: index < visibleWordsCount ? 'slideUpFade 0.8s ease-out forwards' : 'none' 
              }}>
                {word}
              </span>
            ))}
          </div>
          {showEnterButton && (
            <div style={{ position: 'fixed', bottom: '60px', left: '50%', transform: 'translateX(-50%)' }}>
              <button
                onClick={startInput}
                style={{
                  animation: 'slideUpFade 0.6s ease-out forwards',
                  padding: '13px 30px 12px',
                  fontFamily: '"Baskerville Old Face", Baskerville, Georgia, serif',
                  fontSize: '19px',
                  letterSpacing: '0.7px',
                  borderRadius: '7px',
                  background: 'linear-gradient(145deg, #373737, #171717)',
                  color: '#f0f0f0',
                  border: '1px solid #656565',
                  boxShadow: '0 7px 18px rgba(0, 0, 0, 0.38), inset 0 1px 0 rgba(255, 255, 255, 0.15), inset 0 -1px 0 rgba(0, 0, 0, 0.5)',
                  textShadow: '0 1px 2px #000',
                  cursor: 'pointer',
                }}
              >
                Enter Scramble
              </button>
            </div>
          )}
        </>
      )}

      {mode === 'input' && isCubeComplete && !isSolveMode && (
        <>
          <button
            onClick={startSolve}
            style={{ position: 'fixed', bottom: '60px', left: '50%', transform: 'translateX(-50%)', padding: '13px 38px 12px', fontFamily: '"Baskerville Old Face", Baskerville, Georgia, serif', fontSize: '19px', letterSpacing: '0.7px', borderRadius: '7px', background: 'linear-gradient(145deg, #373737, #171717)', color: '#f0f0f0', border: '1px solid #656565', boxShadow: '0 7px 18px rgba(0, 0, 0, 0.38), inset 0 1px 0 rgba(255, 255, 255, 0.15)', textShadow: '0 1px 2px #000', cursor: 'pointer', zIndex: 11 }}
          >
            Solve
          </button>
          {moveInputError && (
            <div style={{ position: 'fixed', bottom: '22px', left: '50%', transform: 'translateX(-50%)', color: moveInputError.startsWith('Solution') || moveInputError.startsWith('Cube is') ? '#9bcf9b' : '#e79797', fontSize: '13px', zIndex: 11, whiteSpace: 'nowrap' }}>
              {moveInputError}
            </div>
          )}
        </>
      )}

      {mode === 'input' && isSolveMode && (
        <>
          {!isSolutionFinished && (
            <div className="solver-panel">
          <div style={{ marginBottom: '9px', textAlign: 'center', color: '#dcdcdc', fontFamily: 'Baskerville, Georgia, serif', fontSize: '15px' }}>Green front · Yellow top · Orange right</div>
          <div className="solver-panel-grid">
            <button onClick={switchToManual} disabled={isAutoRunning && !isPaused} style={{ padding: '8px 0', borderRadius: '5px', background: '#2b2b2b', color: '#f2f2f2', border: '1px solid #666', cursor: 'pointer', opacity: (isAutoRunning && !isPaused) ? 0.45 : 1 }}>Manual</button>
            <button onClick={() => changeSpeed(3000)} disabled={isAutoRunning && !isPaused} style={{ padding: '8px 0', borderRadius: '5px', background: '#2b2b2b', color: '#f2f2f2', border: '1px solid #666', cursor: 'pointer', opacity: (isAutoRunning && !isPaused) ? 0.45 : 1 }}>Slow</button>
            <button onClick={() => changeSpeed(2000)} disabled={isAutoRunning && !isPaused} style={{ padding: '8px 0', borderRadius: '5px', background: '#2b2b2b', color: '#f2f2f2', border: '1px solid #666', cursor: 'pointer', opacity: (isAutoRunning && !isPaused) ? 0.45 : 1 }}>Medium</button>
            <button onClick={() => changeSpeed(1000)} disabled={isAutoRunning && !isPaused} style={{ padding: '8px 0', borderRadius: '5px', background: '#2b2b2b', color: '#f2f2f2', border: '1px solid #666', cursor: 'pointer', opacity: (isAutoRunning && !isPaused) ? 0.45 : 1 }}>Fast</button>
          </div>
          {isAutoRunning && (
            <button onClick={togglePlayback} style={{ display: 'block', margin: '9px auto 0', padding: '7px 18px', borderRadius: '5px', background: '#303030', color: '#fff', border: '1px solid #666', cursor: 'pointer' }}>
              {isPaused ? 'Play' : 'Pause'}
            </button>
          )}
          <div style={{ marginTop: '10px', padding: '8px', overflowX: 'auto', whiteSpace: 'nowrap', background: '#1d1d1d', borderRadius: '6px', border: '1px solid #383838' }}>
            {solutionMoves.map((move, index) => {
              const completed = index < moveIndex;
              const canSeek = isPaused && !isMoveBusy && completed;
              return (
                <button
                  key={`${move}-${index}`}
                  ref={index === moveIndex ? activeMoveRef : null}
                  onClick={() => seekToCompletedMove(index + 1)}
                  disabled={!canSeek}
                  title={canSeek ? `Return to after move ${index + 1}` : 'Pause after a move to revisit it'}
                  style={{ minWidth: '36px', marginRight: '5px', padding: '6px 7px', borderRadius: '4px', background: index === moveIndex ? '#697f46' : completed ? '#3e5132' : '#292929', color: '#f1f1f1', border: '1px solid #666', cursor: canSeek ? 'pointer' : 'default', opacity: completed ? 1 : 0.55 }}
                >
                  {move}
                </button>
              );
            })}
          </div>
          <div style={{ marginTop: '10px', textAlign: 'center', color: '#bcbcbc', fontSize: '13px' }}>Move {moveIndex} of {solutionMoves.length}</div>
          {runMode === 'manual' && (
            <div style={{ display: 'flex', justifyContent: 'center', gap: '8px', marginTop: '8px' }}>
              <button onClick={() => runManualMove('previous')} disabled={isMoveBusy || moveIndex === 0} style={{ padding: '8px 18px', borderRadius: '5px', background: '#303030', color: '#fff', border: '1px solid #666', cursor: 'pointer', opacity: isMoveBusy || moveIndex === 0 ? 0.45 : 1 }}>Previous</button>
              <button onClick={() => runManualMove('next')} disabled={isMoveBusy || moveIndex >= solutionMoves.length} style={{ padding: '8px 18px', borderRadius: '5px', background: '#303030', color: '#fff', border: '1px solid #666', cursor: 'pointer', opacity: isMoveBusy || moveIndex >= solutionMoves.length ? 0.45 : 1 }}>Next</button>
            </div>
          )}
          {isAutoRunning && <div style={{ marginTop: '8px', textAlign: 'center', color: '#9bcf9b', fontSize: '13px' }}>Running solution...</div>}
            </div>
          )}

          {isSolutionFinished && (solutionMoves.length === 0 || (solutionMoves.length === 1 && solutionMoves[0].startsWith('T'))) && (
            <div className="responsive-subtitle" style={{
              position: 'fixed',
              top: '15%',
              left: '50%',
              transform: 'translateX(-50%)'
            }}>
              <div style={{ animation: 'slideUpFade 0.6s ease-out forwards' }}>
                {solutionMoves.length === 0 ? 'The cube is already solved.' : solutionMoves[0]}
              </div>
            </div>
          )}

          {showNewScrambleButton && (
            <div style={{ position: 'fixed', bottom: '60px', left: '50%', transform: 'translateX(-50%)' }}>
              <button
                onClick={startInput}
                style={{
                  animation: 'slideUpFade 0.6s ease-out forwards',
                  padding: '13px 30px 12px',
                  fontFamily: '"Baskerville Old Face", Baskerville, Georgia, serif',
                  fontSize: '19px',
                  letterSpacing: '0.7px',
                  borderRadius: '7px',
                  background: 'linear-gradient(145deg, #373737, #171717)',
                  color: '#f0f0f0',
                  border: '1px solid #656565',
                  boxShadow: '0 7px 18px rgba(0, 0, 0, 0.38), inset 0 1px 0 rgba(255, 255, 255, 0.15), inset 0 -1px 0 rgba(0, 0, 0, 0.5)',
                  textShadow: '0 1px 2px #000',
                  cursor: 'pointer',
                  whiteSpace: 'nowrap',
                }}
              >
                Enter New Scramble
              </button>
            </div>
          )}
        </>
      )}

      {mode === 'input' && !isSolveMode && (
      <>
      {/* Navigation dots — up/down/left/right of the currently viewed face */}
      <button
        onClick={() => navigate('up')}
        style={{ ...dotStyle(CENTER_COLOR[neighbors.up]), position: 'fixed', top: 'calc(50% - 22vh)', left: '50%', transform: 'translate(-50%, -50%)' }}
      />
      <button
        onClick={() => navigate('down')}
        style={{ ...dotStyle(CENTER_COLOR[neighbors.down]), position: 'fixed', top: 'calc(50% + 22vh)', left: '50%', transform: 'translate(-50%, -50%)' }}
      />
      <button
        onClick={() => navigate('left')}
        style={{ ...dotStyle(CENTER_COLOR[neighbors.left]), position: 'fixed', top: '50%', left: 'calc(50% - 22vh)', transform: 'translate(-50%, -50%)' }}
      />
      <button
        onClick={() => navigate('right')}
        style={{ ...dotStyle(CENTER_COLOR[neighbors.right]), position: 'fixed', top: '50%', left: 'calc(50% + 22vh)', transform: 'translate(-50%, -50%)' }}
      />

      {activePopup && (
        <>
          <div onClick={() => setActivePopup(null)} style={{ position: 'fixed', inset: 0, zIndex: 5 }} />
          <div
            className="popup-grid"
            style={{
              position: 'fixed',
              left: activePopup.x,
              top: activePopup.y,
              transform: 'translate(-50%, -110%)',
              zIndex: 10,
            }}
          >
            {COLORS.map((hex) => {
              const isCurrent = cube[activePopup.face][activePopup.stickerIdx] === hex;
              const disabled = counts[hex] >= 9 && !isCurrent;
              return (
                <button
                  key={hex}
                  disabled={disabled}
                  onClick={() => applyColor(hex)}
                  style={{
                    width: '28px',
                    height: '28px',
                    borderRadius: '5px',
                    backgroundColor: hex,
                    border: isCurrent ? '2px solid white' : '1px solid #555',
                    opacity: disabled ? 0.3 : 1,
                    cursor: disabled ? 'not-allowed' : 'pointer',
                  }}
                />
              );
            })}
            <button
              onClick={() => applyColor(UNPAINTED)}
              title="Clear sticker"
              style={{ width: '28px', height: '28px', borderRadius: '5px', background: '#2a2a2a', border: '1px solid #555', color: '#999', cursor: 'pointer' }}
            >
              ×
            </button>
          </div>
        </>
      )}
      </>
      )}
    </div>
  );
}

export default App;
