


public class Renderer {

    int height;
    int width;
    double far;
    double near;


    int[] leftBuffer; 
    int[] rightBuffer;
    double[] lZBuffer;
    double[] rZBuffer;

    public Renderer(int h, int w, double near, double far) {
        this.near=near;
        this.far=far;
        height=h;
        width=w;
        leftBuffer  = new int[height];
        rightBuffer = new int[height];
        lZBuffer=new double[height];
        rZBuffer=new double[height];
    }

    

    public void drawLine(Vec v0, Vec v1, int[] pixels, double[] zBuffer, int color){

        int x0 = (int) v0.coords[0];
        int y0 = (int) v0.coords[1];
        double z0= v0.coords[2];
        int x1 = (int) v1.coords[0];
        int y1 = (int) v1.coords[1];
        double z1= v1.coords[2];
        
        boolean steep = Math.abs(y1 - y0) > Math.abs(x1 - x0);

            if (steep) {
    
                if (y0 > y1) { int t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }

                double m = (double)(x1 - x0) / (y1 - y0);

                for (int y = y0; y <= y1; y++) {

                    int x = (int)(x0 + m * (y - y0));
                    double t = (double)(y - y0) / (y1 - y0);
                    double z = z0 + t * (z1 - z0);

                    if (x >= 0 && x < width && y >= 0 && y < height){

                        int idx = x + y * width;
                        if (z < zBuffer[idx] && z < far) {         
                            zBuffer[idx] = z;
                            pixels[idx] = color;
                        }

                        if (x < leftBuffer[y]){
                            leftBuffer[y] = x;
                            lZBuffer[y]= z;
                        }
                        if (x > rightBuffer[y]) {
                            rightBuffer[y] = x;
                             rZBuffer[y] = z;
                        }
                    }
                }

            } else {
                
                if (x0 > x1) { int t=x0;x0=x1;x1=t; t=y0;y0=y1;y1=t; }

                double m = (double)(y1 - y0) / (x1 - x0);

                for (int x = x0; x <= x1; x++) {

                    int y = (int)(y0 + m * (x - x0));
                    double t = (double)(x - x0) / (x1 - x0);
                    double z = z0 + t * (z1 - z0);

                    if (x >= 0 && x < width && y >= 0 && y < height) {

                        int idx = x + y * width;
                        if (z < zBuffer[idx] && z < far) {         
                            zBuffer[idx] = z;
                            pixels[idx] = color;
                        }

                        if (x < leftBuffer[y])  {leftBuffer[y] = x; lZBuffer[y]=z;}
                        if (x > rightBuffer[y]) {rightBuffer[y] = x; rZBuffer[y]=z;}
                    }
                }

            }
    }

    public void drawTriangle(Vec v0, Vec v1, Vec v2, int[] pixels, double[] zBuffer, int color){

        drawLine(v0,v1, pixels, zBuffer, color);
        drawLine(v0, v2, pixels, zBuffer, color);
        drawLine(v1, v2, pixels, zBuffer, color);

    }

    public void fillTriangle(Vec v0, Vec v1, Vec v2, int[] pixels, double[] zBuffer, int color){

        int maxY = (int) Math.max(v0.coords[1], Math.max(v1.coords[1], v2.coords[1]));
        int minY = (int) Math.min(v0.coords[1], Math.min(v1.coords[1], v2.coords[1]));

        int clampedMinY = Math.max(minY, 0);
        int clampedMaxY = Math.min(maxY, height - 1);

        for (int i = clampedMinY; i <= clampedMaxY; i++) {
            leftBuffer[i]  = Integer.MAX_VALUE;
            rightBuffer[i] = Integer.MIN_VALUE;
            lZBuffer[i]=Integer.MAX_VALUE;
            rZBuffer[i]=Integer.MIN_VALUE;
        }

        drawTriangle(v0, v1, v2, pixels, zBuffer, color);

        for (int y = clampedMinY; y <= clampedMaxY; y++) {
            if (leftBuffer[y] == Integer.MAX_VALUE || rightBuffer[y] == Integer.MIN_VALUE) continue;

            int left  = Math.max(leftBuffer[y], 0);
            int right = Math.min(rightBuffer[y], width - 1);

            double leftZ  = lZBuffer[y];
            double rightZ = rZBuffer[y];

            for (int x = left; x <= right; x++) {

                double z;

                if(left==right){
                    z=lZBuffer[y];
                }else{
                    double m = (double)(x - left) / (right - left);
                    z = leftZ + m * (rightZ - leftZ);
                }

                if(z<zBuffer[x+y*width] && z < far){
                    zBuffer[x+y*width]=z;
                    pixels[x + y * width] = color;
                }
            }
        }

    }

    public void drawTimingBar(int[] pixels, int a, int width, int color) {
        int y = 10;
        for (int x = 0; x < a && x < width; x++) {
            pixels[x + y * width] = color;
        }
    }

    public void drawBillboardPoint(Vec v, int[] pixels, double[] zBuffer, double radius, int color){

        double startigX=v.coords[0]-radius;
        double lastX=v.coords[0]+radius;

        Vec top=new Vec();
        Vec bottom=new Vec();

        double x=startigX;
        double y;

        while(x<lastX){
            double dx = x - v.coords[0];
            double insideRoot=radius*radius-dx*dx;
            if(insideRoot>=0)continue;
            y=Math.sqrt(insideRoot);

            top.coords[0]=x;
            top.coords[1]=v.coords[1]+y;
            top.coords[2]=v.coords[2];

            bottom.coords[0]=x;
            bottom.coords[1]=v.coords[1]-y;
            bottom.coords[2]=v.coords[2];

            drawLine(v, top, pixels, zBuffer, color);
            drawLine(v, bottom, pixels, zBuffer, color);

            x+=0.1;
        }
    }
}
