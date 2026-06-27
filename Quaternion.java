public class Quaternion {
    double[] coords=new double[4];

    public Quaternion(double r, double x, double y, double z) {
        coords[0]=r;
        coords[1]=x;
        coords[2]=y;
        coords[3]=z;
    }

    public static Quaternion inverse(Quaternion q){
        return new Quaternion(q.coords[0], -q.coords[1], -q.coords[2], -q.coords[3]);
    }

    public static Quaternion angleAxis(Vec axis, double angle){
        return new Quaternion(Math.cos(angle/2), Math.sin(angle/2)*axis.coords[0], Math.sin(angle/2)*axis.coords[1], Math.sin(angle/2)*axis.coords[2]);
    }    

    public static Quaternion mult(Quaternion q, Quaternion p) {
        return new Quaternion(
            q.coords[0] * p.coords[0] - q.coords[1] * p.coords[1] - q.coords[2] * p.coords[2] - q.coords[3] * p.coords[3],
            q.coords[0] * p.coords[1] + q.coords[1] * p.coords[0] + q.coords[2] * p.coords[3] - q.coords[3] * p.coords[2],
            q.coords[0] * p.coords[2] - q.coords[1] * p.coords[3] + q.coords[2] * p.coords[0] + q.coords[3] * p.coords[1],
            q.coords[0] * p.coords[3] + q.coords[1] * p.coords[2] - q.coords[2] * p.coords[1] + q.coords[3] * p.coords[0]
        );
    }
}
