
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

import javax.swing.*;

public class SecondFrame extends JFrame implements KeyListener, MouseListener, MouseMotionListener, MouseWheelListener, Runnable{

    Canvas panel=new Canvas();

    final int height = 1000;
    final int width = 1000;

    BufferedImage frame = new BufferedImage(height, width, BufferedImage.TYPE_INT_RGB);
    int[] pixels = ((DataBufferInt) frame.getRaster().getDataBuffer()).getData();
    double[] zBuffer = new double[height * width];
    
    double unit = 15;

    double rotationAngle = 0.0005;
    double maxRotAccel = 0.06;
    Boolean accelerateRot = true;

    Formula heightCos = new Formula() {
        public double calc(double x, double z) {
            return Math.sin(x) + Math.cos(z);
        }
    };

    Formula plasmaSea = new Formula() {
        public double calc(double x, double z) {
            double val = Math.sin(x * 0.3);
            val += Math.sin((z * 0.3) / 2.0);
            val += Math.sin((x + z) * 0.3);
            double zoom = Math.sqrt(x * x + z * z);
            val += Math.sin(zoom * 0.3);
            return val;
        }
    };

    Formula rollingHills = new Formula() {
        public double calc(double x, double z) {
            // Layer 1: Large rolling hills (low frequency, high amplitude)
            double hills = Math.sin(x * 0.2) * Math.cos(z * 0.2) * 2.0;

            // Layer 2: Smaller surface ripples (high frequency, low amplitude)
            double ripples = Math.sin(x * 0.8 + z * 0.5) * 0.5;

            return hills + ripples;
        }
    };

    Formula spikyPeaks = new Formula() {
        @Override
        public double calc(double x, double z) {
            // Using absolute values creates sharp "creases" or ridges
            double layer1 = Math.abs(Math.sin(x * 0.4) * Math.cos(z * 0.4));

            // Layering a faster absolute wave adds jagged detail
            double layer2 = Math.abs(Math.sin(x * 1.1 + z * 0.7)) * 0.4;

            // Raising to a power sharpens the peaks and flattens the ground
            return -Math.pow(layer1 + layer2, 2.2) * 3.0;
        }
    };

    Formula jaggedSpikes = new Formula() {
        @Override
        public double calc(double x, double z) {
            // High frequency interference creates a chaotic, spiky appearance
            double spikes = Math.sin(x * 2.5) * Math.sin(z * 2.5);
            spikes += Math.cos(x * 4.0 + z * 2.0) * 0.3;

            // Clamp the bottom so spikes only go upward
            return -Math.max(0, spikes * 1.5);
        }
    };

    Formula chaoticTerrain = new Formula() {
        @Override
        public double calc(double x, double z) {
            // 1. High-frequency noise base
            double noise = Math.sin(x * 0.9) * Math.cos(z * 1.3) +
                    Math.sin(x * 2.5 + z * 1.7) * 0.5;

            // 2. Normalize to a 0.0 to 1.0 range
            // Adding 1.5 because the noise range above is approx -1.5 to 1.5
            double normalized = (noise + 1.5) / 3.0;

            // 3. The Power Function
            // Raising to a high power makes values near 1.0 (peaks) stay high
            // while values near 0.0 (valleys/crevices) drop significantly.
            double result = Math.pow(normalized, 4.0) * 12.0;

            // 4. Offset to center the terrain vertically
            // This makes the "floor" drop into deep crevices while peaks shoot up.
            return -(result - 2.0);
        }
    };

    double frameRate = 45;
    int frameCalc = (int) (1000 / frameRate);
    int barProgress = 10;
    double time = 0;

    int screen = 0;
    Boolean pause = true;
    Boolean fill=true;

    Camera camera = new Camera(0, 20, -30);

    Vec camOrigin= new Vec(0,20,-30);
    Vec light = new Vec(0, -10, -20);
    Vec worldOrigin=new Vec(0, 0, 0);
    Vec worldXAxis=new Vec(1,0,0);
    Vec worldYAxis=new Vec(0,1,0);
    Vec worldZAxis=new Vec(0,0,1);

    double fNear = 0.1;
    double fFar = width;
    double FOV = 90;
    double FOVRad = 1 / Math.tan(FOV * 0.5 / 180 * Math.PI);
    double aspectRatio = height / width;

    Matrix projectionMatrix = new Matrix(new double[][] {
            new double[] { aspectRatio * FOVRad, 0, 0, 0 },
            new double[] { 0, FOVRad, 0, 0 },
            new double[] { 0, 0, fFar / (fFar - fNear), 1 },
            new double[] { 0, 0, -fFar * fNear / (fFar - fNear), 0 }
    });

    int baseColor = (120 << 16) | (198 << 8) | 233;

    MeshV2 mesh = MeshV2.cubeGen(baseColor, unit, worldOrigin);
    MeshV2 mesh2 = MeshV2.heightMapGen(41, 41, plasmaSea, baseColor, worldOrigin);
    MeshV2 icosahedron=MeshV2.icosphereGen(unit, worldOrigin, baseColor, 3);
    MeshV2 currentMesh=mesh;

    MeshV2 lightCube= MeshV2.cubeGen(((255 << 16) | (255 << 8) | 255), 1, light);

    Thread rederingThread;
    Renderer renderer;
    Boolean cull;

    public SecondFrame() throws HeadlessException {
        panel.setPreferredSize(new Dimension(width, height));
        this.setBackground(Color.BLACK);
        this.add(panel);
        this.pack();
        panel.addMouseListener(this);
        panel.addMouseMotionListener(this);
        panel.addMouseWheelListener(this);
        panel.addKeyListener(this);
        this.setVisible(true);
        panel.createBufferStrategy(2);
        panel.setFocusable(true);
        panel.requestFocus();
        renderer=new Renderer(height, width, fNear, fFar);
        rederingThread=new Thread(this);
        rederingThread.start();
    }

    @Override
    public void run(){

        BufferStrategy bs = panel.getBufferStrategy();

        while(true){

            //cull=currentMesh!=mesh2;

            if (pendingRotX != 0) {
                camera.betterCamSpin(camera.xAxis, worldOrigin, pendingRotX);
                pendingRotX = 0;
            }
            if (pendingRotY != 0) {
                camera.betterCamSpin(worldYAxis, worldOrigin, pendingRotY);
                pendingRotY = 0;
            }

            Arrays.fill(pixels, 0x000000);
            Arrays.fill(zBuffer, Double.MAX_VALUE);
            
            time+=0.03;

            if(!pause){
                switch (Math.abs(screen % 7)) {
                    case 0:
                        spinRoutine(mesh);
                        break;
                    case 1:
                        heightMapRoutine(plasmaSea);
                        break;
                    case 2:
                        heightMapRoutine(rollingHills);
                        break;
                    case 3:
                        heightMapRoutine(jaggedSpikes);
                        break;
                    case 4:
                        heightMapRoutine(spikyPeaks);
                        break;
                    case 5:
                        heightMapRoutine(chaoticTerrain);
                        break;
                    case 6:
                        spinRoutine(icosahedron);
                        break;
                }
            }

            barProgress = (barProgress < width) ? barProgress + frameCalc : 10;
            renderer.drawTimingBar(pixels, barProgress, width, 0x888888);
            currentMesh.drawMesh(renderer, pixels, zBuffer, projectionMatrix, width, height, camera, light, true, fill, false, false);
            lightCube.drawMesh(renderer, pixels, zBuffer, projectionMatrix, width, height, camera, light, true,true, true, false);

            Graphics g =bs.getDrawGraphics();
            g.drawImage(frame, 0, 0, null);
            g.dispose();
            bs.show();

            try {
                Thread.sleep(frameCalc);
            } catch(InterruptedException e){
                IO.println("Error in thread");
            }
            
        }
    }

     public void accelBool(double d) {
        if (d > maxRotAccel) {
            accelerateRot = false;
        } else if (d < -maxRotAccel) {
            accelerateRot = true;
        }
    }

    public void rotationAccelChange() {
        accelBool(rotationAngle);
        rotationAngle = (accelerateRot) ? rotationAngle + 0.0003 : rotationAngle - 0.0003;
    }

    public void spinRoutine(MeshV2 m) {
        rotationAccelChange();
        m.rotate(worldXAxis, rotationAngle);
        m.rotate(worldYAxis, -rotationAngle/2);
        m.rotate(worldZAxis, -rotationAngle);
    }

    public void heightMapRoutine(Formula f) {
        mesh2.heightMapUpdate(f, time);
    }

    Set<Integer> keys = new HashSet<Integer>();

    @Override
    public void keyPressed(KeyEvent e) {
        keys.add(e.getKeyCode());
        keyHandling(e);
    }

    volatile double pendingRotX = 0;
    volatile double pendingRotY = 0;
    volatile double pendingRotZ = 0;

    public void keyHandling(KeyEvent e){
        for (Integer key :keys){
            switch (key) {
                case KeyEvent.VK_1:
                    screen = (screen > 5) ? 0 : screen + 1;
                    switch (screen){
                        case 0:
                            currentMesh=mesh;
                            break;
                        case 1,2,3,4,5:
                            currentMesh=mesh2;
                            break;
                        case 6:
                            currentMesh=icosahedron;
                            break;
                    }
                    break;
                case KeyEvent.VK_2:
                    screen = (screen < 1) ? 6 : screen - 1;
                    switch (screen){
                        case 0:
                            currentMesh=mesh;
                            break;
                        case 1,2,3,4,5:
                            currentMesh=mesh2;
                            break;
                        case 6:
                            currentMesh=icosahedron;
                            break;
                        }
                    break;
                case KeyEvent.VK_UP:
                    if(e.isShiftDown()){
                        light.coords[2]+=0.5;
                        lightCube.move(light);
                    }else{
                        Vec.pointSpin(worldXAxis, camera.target, light, -Math.PI / 90);
                        lightCube.move(light);
                    }
                    break;
                case KeyEvent.VK_DOWN:
                    if(e.isShiftDown()){
                        light.coords[2]-=0.5;
                        lightCube.move(light);
                    }else{
                    Vec.pointSpin(worldXAxis, camera.target, light, Math.PI / 90);
                    lightCube.move(light);
                    }
                    break;
                case KeyEvent.VK_RIGHT:
                    if(e.isShiftDown()){
                        light.coords[0]+=0.5;
                        lightCube.move(light);
                    }else{
                    Vec.pointSpin(worldYAxis, camera.target, light, -Math.PI / 90);
                    lightCube.move(light);
                    }
                    break;
                case KeyEvent.VK_LEFT:
                    if(e.isShiftDown()){
                        light.coords[0]-=0.5;
                        lightCube.move(light);
                    }else{
                    Vec.pointSpin(worldYAxis, camera.target, light, Math.PI / 90);
                    lightCube.move(light);
                    }
                    break;
                case KeyEvent.VK_W:
                    camera.betterCamSpin(worldXAxis, worldOrigin,Math.PI/90);
                    break;
                case KeyEvent.VK_A:
                    camera.betterCamSpin(worldYAxis, worldOrigin, Math.PI/90);
                    break;
                case KeyEvent.VK_D:
                    camera.betterCamSpin(worldYAxis, worldOrigin, -Math.PI/90);
                    break;
                case KeyEvent.VK_S:
                    camera.betterCamSpin(worldXAxis, worldOrigin, -Math.PI/90);
                    break;
                case KeyEvent.VK_ESCAPE:
                    pause = (pause) ? false : true;
                    break;
                case KeyEvent.VK_L:
                    light.coords[0] = 0;
                    light.coords[1] = -10;
                    light.coords[2] = -20;
                    lightCube.move(light);
                    break;
                case KeyEvent.VK_P:
                    System.exit(0);
                    break;
                case KeyEvent.VK_R:

                    camera=new Camera(camOrigin.coords[0],camOrigin.coords[1],camOrigin.coords[2]);
                    camera.calcViewMatrix(camera.target);

                    break;

                case KeyEvent.VK_SPACE:
                    camera.origin.coords[1]+=0.1;
                    camera.calcViewMatrix(camera.lookDirect);
                    break;
                case KeyEvent.VK_X:
                    camera.origin.coords[1]-=0.1;
                    camera.calcViewMatrix(camera.lookDirect);
                    break;
                case KeyEvent.VK_F:
                    fill=(fill)?false:true;
                    break;
                default:
                    break;
            }
        }
    }

    @Override
    public void keyReleased(KeyEvent e) {
        keys.remove(e.getKeyCode());
    }

    @Override
    public void keyTyped(KeyEvent e) {
        
    }

    private Point lastMousePosition;
    double sens=0.005;

    @Override
    public void mouseDragged(MouseEvent e) {
        if(lastMousePosition==null){
            lastMousePosition=e.getPoint();
            return;
        }else if(e.isShiftDown()){
            int x=e.getX();
            int y=e.getY();

            int dx=x-lastMousePosition.x;
            int dy=y-lastMousePosition.y;

            pendingRotX+=dy*sens;
            pendingRotY+=-dx*sens;

             lastMousePosition=e.getPoint();
            repaint();
        }
    }



    @Override
    public void mouseClicked(MouseEvent e) {
        int mouseX=e.getX();
        int mouseY=e.getY();
        
    }

    @Override
    public void mouseEntered(MouseEvent e) {

    }

    @Override
    public void mousePressed(MouseEvent e) {
        lastMousePosition=e.getPoint();
    }

    @Override
    public void mouseReleased(MouseEvent e) {

    }

    @Override
    public void mouseExited(MouseEvent e) {
    }

    @Override
    public void mouseMoved(MouseEvent e) {
    }

    double zoomCoeff=0.05;
    
    @Override
    public void mouseWheelMoved(MouseWheelEvent e) {
        Vec distance = Vec.subs(camera.target, camera.origin);
        camera.origin.coords[0]=camera.origin.coords[0]+distance.coords[0]*zoomCoeff*e.getWheelRotation();
        camera.origin.coords[1]=camera.origin.coords[1]+distance.coords[1]*zoomCoeff*e.getWheelRotation();
        camera.origin.coords[2]=camera.origin.coords[2]+distance.coords[2]*zoomCoeff*e.getWheelRotation();
        camera.calcViewMatrix(camera.target);
    }

    @Override
    protected void processMouseMotionEvent(MouseEvent e) {
        super.processMouseMotionEvent(e);
    }

}
