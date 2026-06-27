
public class Matrix {
    double[][] values= new double[4][4];

    public Matrix(double[][] m){
        this.values=m;
    }
    
    public Matrix(){
    }

    public static void mult(Vec v, Matrix m){
        double[] results = new double[4];
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                results[i]+=v.coords[j]*m.values[j][i];
            }
        }
        System.arraycopy(results, 0, v.coords, 0, 4);
    }

    public static Vec multNew(Vec v, Matrix m){
        Vec newVec=new Vec();
        for(int i=0;i<4;i++){
            for(int j=0;j<4;j++){
                newVec.coords[i]+=v.coords[j]*m.values[j][i];
            }
        }
        return newVec;
    }

    public static void projecTransform(Vec v, Matrix m){
        Matrix.mult(v, m);
        if(v.coords[3]!=0){
            Vec.Div(v, v.coords[3]);
        }
    }

    public static Matrix getRotationZ(double angle) {
        double[][] m = new double[4][4];
        double cosTheta = Math.cos(angle);
        double sinTheta = Math.sin(angle);

        // Applying Rotation Z logic from your snippet
        m[0][0] = cosTheta;
        m[0][1] = sinTheta;
        m[1][0] = -sinTheta;
        m[1][1] = cosTheta;
        m[2][2] = 1.0;
        m[3][3] = 1.0;

        return new Matrix(m);
    }

    public static Matrix getRotationX(double angle) {
        double[][] m = new double[4][4];
        // Image shows fTheta * 0.5f
        double adjustedAngle = angle;
        double cosTheta = Math.cos(adjustedAngle);
        double sinTheta = Math.sin(adjustedAngle);
        
        // Applying Rotation X logic from your snippet
        m[0][0] = 1.0;
        m[1][1] = cosTheta;
        m[1][2] = sinTheta;
        m[2][1] = -sinTheta;
        m[2][2] = cosTheta;
        m[3][3] = 1.0;
    
        return new Matrix(m);
    }

    public static Matrix getRotationY(double angle) {
       double[][] m = new double[4][4];

       double cos = Math.cos(angle);
       double sin = Math.sin(angle);
       
        m[0][0] = cos;  m[0][2] = sin;
        m[1][1] = 1;
        m[2][0] = -sin; m[2][2] = cos;
        m[3][3] = 1;

       return new Matrix(m);
    }
}
