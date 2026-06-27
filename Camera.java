public class Camera {
    Vec origin;
    Vec zAxis;
    Vec yAxis;
    Vec xAxis;

    Vec lookDirect = new Vec();

    Vec target = new Vec();

    Matrix viewMatrix = new Matrix();

    public Camera(double x, double y, double z) {
        origin = new Vec(x, y, z);
        zAxis = new Vec(0, 0, 1);
        yAxis = new Vec(0, 1, 0);
        xAxis = new Vec(1, 0, 0);
        lookDirect = new Vec(0, 0, 0);
        target = new Vec(0, 0, 0);
        this.calcViewMatrix(this.target);
    }

    public void calcViewMatrix(Vec target) {
        Vec newZAxis = Vec.normaliseVec(Vec.subs(target, origin));

        Vec temp = Vec.mult(newZAxis, Vec.scalarProduct(yAxis, newZAxis));
        Vec newYAxis = Vec.normaliseVec(Vec.subs(yAxis, temp));

        Vec newXAxis = Vec.vecProduct(newYAxis, newZAxis);

        viewMatrix.values[0] = new double[] { newXAxis.coords[0], newXAxis.coords[1], newXAxis.coords[2], 0 };
        viewMatrix.values[1] = new double[] { newYAxis.coords[0], newYAxis.coords[1], newYAxis.coords[2], 0 };
        viewMatrix.values[2] = new double[] { newZAxis.coords[0], newZAxis.coords[1], newZAxis.coords[2], 0 };
        viewMatrix.values[3] = new double[] { -Vec.scalarProduct(newXAxis, origin), -Vec.scalarProduct(newYAxis, origin),
                -Vec.scalarProduct(newZAxis, origin), 1 };

        lookDirect=Vec.normaliseVec(Vec.subs(target, zAxis));
    }

    //DEPRECIATED SPIN METHODS

/*    public void camSpinY(Vec center, double angle){
        
        double relX = this.origin.coords[0] - center.coords[0];
        double relZ = this.origin.coords[2] - center.coords[2];

        double cosA = Math.cos(angle);
        double sinA = Math.sin(angle);

        double newX = relX * cosA - relZ * sinA;
        double newZ = relX * sinA + relZ * cosA;

        this.origin.coords[0] = center.coords[0] + newX;
        this.origin.coords[2] = center.coords[2] + newZ;

        lookDirect=Vec.normaliseVec(Vec.subs(target, origin));
        this.calcViewMatrix(this.target);
    }

    public void camSpinX(Vec center, double angle){
        
        double relZ = this.origin.coords[2] - center.coords[2];
        double relY = this.origin.coords[1] - center.coords[1];

        double cosA = Math.cos(angle);
        double sinA = Math.sin(angle);

        double newZ = relZ * cosA - relY * sinA;
        double newY = relZ * sinA + relY * cosA;

        this.origin.coords[2] = center.coords[2] + newZ;
        this.origin.coords[1] = center.coords[1] + newY;

        lookDirect=Vec.normaliseVec(Vec.subs(target, origin));
        this.calcViewMatrix(this.target);
    }  */

    public void pointSpin(Vec axis, Vec center, Vec point, double angle){

        Vec camRelative=new Vec(point.coords[0]-center.coords[0],
            point.coords[1]-center.coords[1],point.coords[2]-center.coords[2]);

        Quaternion p=new Quaternion(0, camRelative.coords[0],
             camRelative.coords[1], camRelative.coords[2]);

        Quaternion q=Quaternion.angleAxis(axis, angle);
        Quaternion multProd=Quaternion.mult(Quaternion.mult(q, p), Quaternion.inverse(q));

        point.coords[0] = multProd.coords[1]+center.coords[0];
        point.coords[1] = multProd.coords[2]+center.coords[1];
        point.coords[2] = multProd.coords[3]+center.coords[2];
    }

    public void pointSpin(Vec axis, Vec point, double angle){

        Vec camRelative=new Vec(point.coords[0],
            point.coords[1],point.coords[2]);

        Quaternion p=new Quaternion(0, camRelative.coords[0],
             camRelative.coords[1], camRelative.coords[2]);

        Quaternion q=Quaternion.angleAxis(axis, angle);
        Quaternion multProd=Quaternion.mult(Quaternion.mult(q, p), Quaternion.inverse(q));

        point.coords[0] = multProd.coords[1];
        point.coords[1] = multProd.coords[2];
        point.coords[2] = multProd.coords[3];
    }


    public void betterCamSpin(Vec axis, Vec center, double angle){
        pointSpin(axis, center, origin, angle);
        pointSpin(axis, center, zAxis, angle);
        pointSpin(axis, center, yAxis, angle);
        pointSpin(axis, center, xAxis, angle);

        lookDirect=Vec.normaliseVec(Vec.subs(center, origin));
        this.calcViewMatrix(this.target);
    }
}
