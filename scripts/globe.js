import * as THREE from "https://unpkg.com/three@0.158.0/build/three.module.js";

/* ---------- CONTAINER ---------- */
const container = document.getElementById("globe-container");

/* ---------- SCENE ---------- */
const scene = new THREE.Scene();
scene.background = new THREE.Color(0x313131);
/* ---------- CAMERA ---------- */
const camera = new THREE.PerspectiveCamera(
  45,
  container.clientWidth / container.clientHeight,
  0.1,
  1000
);
camera.position.z = 0.3;

/* ---------- RENDERER ---------- */
const renderer = new THREE.WebGLRenderer({ 
    antialias: true,
});
renderer.setSize(container.clientWidth, container.clientHeight);
renderer.setPixelRatio(window.devicePixelRatio);
container.appendChild(renderer.domElement);

/* ---------- GLOBE GROUP ---------- */
const globeGroup = new THREE.Group();
scene.add(globeGroup);

const radius = 0.09;
const segments = 32;

/* ---------- LATITUDE LINES ---------- */
for (let lat = -60; lat <= 60; lat += 35) {

  const r = radius * Math.cos(THREE.MathUtils.degToRad(lat));
  const y = radius * Math.sin(THREE.MathUtils.degToRad(lat));

  const curve = new THREE.EllipseCurve(
    0, 0,
    r, r,
    0, 2 * Math.PI
  );

  const points = curve.getPoints(segments);
  const geometry = new THREE.BufferGeometry().setFromPoints(points);

  const material = new THREE.LineBasicMaterial({
    color: 0xffffff,
    transparent: true,
    opacity: 0.7
  });

  const line = new THREE.LineLoop(geometry, material);
  line.rotation.x = Math.PI / 2;
  line.position.y = y;

  globeGroup.add(line);
}

/* ---------- LONGITUDE LINES ---------- */
for (let lon = 0; lon < 180; lon += 65) {

  const curve = new THREE.EllipseCurve(
    0, 0,
    radius, radius,
    0, 2 * Math.PI
  );

  const points = curve.getPoints(segments);
  const geometry = new THREE.BufferGeometry().setFromPoints(points);

  const material = new THREE.LineBasicMaterial({
    color: 0xffffff,
    transparent: true,
    opacity: 0.7
  });

  const line = new THREE.LineLoop(geometry, material);
  line.rotation.y = THREE.MathUtils.degToRad(lon);

  globeGroup.add(line);
}

/* ---------- OUTER SPHERE ---------- */
const sphere = new THREE.Mesh(
  new THREE.SphereGeometry(radius, 16, 16),
  new THREE.MeshBasicMaterial({
    color: 0xffffff,
    wireframe: true,
    transparent: true,
    opacity: 0.05
  })
);
globeGroup.add(sphere);

/* ---------- ANIMATION ---------- */
function animate() {
  requestAnimationFrame(animate);

  globeGroup.rotation.y += 0.01;
  globeGroup.rotation.x = 0.3;
  globeGroup.rotation.z = Math.sin(Date.now() * 0.002) * 0.2;

  renderer.render(scene, camera);
}

animate();

/* ---------- RESPONSIVE ---------- */
window.addEventListener("resize", () => {
  const width = container.clientWidth;
  const height = container.clientHeight;

  renderer.setSize(width, height);
  camera.aspect = width / height;
  camera.updateProjectionMatrix();
});