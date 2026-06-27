
public class Triangle {
    Vec[] vecs = new Vec[3];
    Vec normal =new Vec();
    Vec centroid=  new Vec();

    public Triangle(Vec... points) {
        vecs = points;
        triangleNormal();
        centroidCalc();
    }

    public Triangle() {
        for (int i = 0; i < 3; i++) {
            this.vecs[i] = new Vec();
        }
    }

    public void triangleNormal() {
        this.normal=Vec.vecProduct(vecs[0], vecs[1], vecs[2]);
    }

    public void centroidCalc(){

        centroid.coords[0]=0;
        centroid.coords[1]=0;
        centroid.coords[2]=0;

        for(Vec v:vecs){
            centroid.coords[0]+=v.coords[0];
            centroid.coords[1]+=v.coords[1];
            centroid.coords[2]+=v.coords[2];
        }

        centroid.coords[0] /= 3;
        centroid.coords[1] /= 3;
        centroid.coords[2] /= 3;
    }

    public static Triangle triOffset(Triangle t, double offset) {
        Triangle tri = new Triangle();
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 4; j++) {
                if (j == 2) {
                    tri.vecs[i].coords[j] = t.vecs[i].coords[j] + offset;
                } else {
                    tri.vecs[i].coords[j] = t.vecs[i].coords[j];
                }
            }
        }
        tri.triangleNormal();
        return tri;
    }

    public void triangleTranslate(Matrix m) {
        for (int i = 0; i < 3; i++) {
            Matrix.projecTransform(this.vecs[i], m);
        }
    }

    public void triangleProject(double width, double height) {
        for (int i = 0; i < 3; i++) {
            vecs[i].coords[0] = (this.vecs[i].coords[0] + 1) * (width) / 2;
            vecs[i].coords[1] = (this.vecs[i].coords[1] + 1) * (height) / 2;
            vecs[i].coords[3]=1;
        }
    }

    public void triangleCameraSpace(Triangle t, Camera camera, Vec target) {
        for (int i = 0; i < 3; i++) {
            System.arraycopy(t.vecs[i].coords, 0, this.vecs[i].coords, 0, 4);
            Matrix.mult(vecs[i], camera.viewMatrix);
        }
        triangleNormal();
        centroidCalc();
    }

    public double depth() {
        return (vecs[0].coords[2] + vecs[1].coords[2] + vecs[2].coords[2]) / 3;
    }

    public static double[] maxCoords(Triangle t){
        double maximumX=-1000000;
        double maximumY=-1000000;
        double maximumZ=-1000000;
            for(Vec v:t.vecs){
                maximumX=(maximumX<=v.coords[0])? v.coords[0]:maximumX;
                maximumY=(maximumY<=v.coords[1])? v.coords[1]:maximumY;
                maximumZ=(maximumZ<=v.coords[2])? v.coords[2]:maximumZ;
            }

        return new double[]{maximumX,maximumY,maximumZ};
    }
}
