public class Vec {
    double[] coords={0,0,0,1};
    Boolean seen=false;

    public Vec(double x,double y,double z) {
        this.coords[0]=x;
        this.coords[1]=y;
        this.coords[2]=z;
        this.coords[3]=1;
    }
    
    public Vec(){}

    public static void Div(Vec v, double div){
        for(int i=0;i<4;i++){
            v.coords[i]=v.coords[i]/div;
        }
    }

    public static double size(Vec v){
        return Math.sqrt(v.coords[0]*v.coords[0]+v.coords[1]*v.coords[1]+v.coords[2]*v.coords[2]);
    }

    public static Vec trueVec(Vec v1, Vec v2){
        Vec newVec=new Vec();
        for(int i=0;i<3;i++){
            newVec.coords[i]=v1.coords[i]-v2.coords[i];
        }
        return newVec;
    }

    public static double scalarProduct(Vec v1, Vec v2){
        double sum=0;
        for(int i=0;i<3;i++){
            sum+=v1.coords[i]*v2.coords[i];
        }
        return sum;
    }

    public static Vec normaliseVec(Vec v){
        double s=Vec.size(v);
        return new Vec(v.coords[0] / s, v.coords[1] / s, v.coords[2] / s);
    }

    public static Vec vecProduct(Vec m1, Vec m2, Vec m3){
        Vec v1=Vec.trueVec(m2, m1);
        Vec v2=Vec.trueVec(m3, m1);

        Vec newVec=new Vec(
            v1.coords[1]*v2.coords[2]-v1.coords[2]*v2.coords[1],
            v1.coords[2]*v2.coords[0]-v1.coords[0]*v2.coords[2],
            v1.coords[0]*v2.coords[1]-v1.coords[1]*v2.coords[0]
        );
        
        return Vec.normaliseVec(newVec);
    }

    public static Vec vecProduct(Vec v1, Vec v2){
        Vec newVec=new Vec(
            v1.coords[1]*v2.coords[2]-v1.coords[2]*v2.coords[1],
            v1.coords[2]*v2.coords[0]-v1.coords[0]*v2.coords[2],
            v1.coords[0]*v2.coords[1]-v1.coords[1]*v2.coords[0]
        );
        
        return Vec.normaliseVec(newVec);
    }

    public static Vec mult(Vec v, double d){
        Vec newVec=new Vec();
        for(int i=0;i<3;i++){
            newVec.coords[i]=d*v.coords[i];
        }
        return newVec;
    }
    

    public static Vec[][] heightMapVecGen(int maxX, int maxZ, Formula yFunction){
        Vec[][] grid=new Vec[maxX][maxZ];
        for (int i = 0; i < maxX; i++) {
            for (int j = 0; j < maxZ; j++) {
                grid[i][j]=new Vec(i, yFunction.calc(i, j), j);
            }
        }
        return grid;
    }

    public void lightSpinY(Vec origin, double angle){
        Vec displacedLight=new Vec();
        for(int i=0;i<3;i++){
            if(i!=1){
                displacedLight.coords[i]=this.coords[i]-origin.coords[i];
            }else{
                displacedLight.coords[i]=this.coords[i];
            }
        }

        double xStore=displacedLight.coords[0];

        displacedLight.coords[0]=Math.cos(angle)*displacedLight.coords[0]-Math.sin(angle)*displacedLight.coords[2];

        displacedLight.coords[2]=Math.cos(angle)*displacedLight.coords[2]+Math.sin(angle)*xStore;

        for(int i=0;i<3;i++){
            if(i!=1){
                displacedLight.coords[i]=origin.coords[i]+displacedLight.coords[i];
                this.coords[i]=displacedLight.coords[i];
            }
        }
    }

    public void lightSpinX(Vec origin, double angle){
        Vec displacedLight=new Vec();
        for(int i=0;i<3;i++){
            if(i!=0){
                displacedLight.coords[i]=this.coords[i]-origin.coords[i];
            }else{
                displacedLight.coords[i]=this.coords[i];
            }
        }

        double yStore=displacedLight.coords[1];

        displacedLight.coords[1]=Math.cos(angle)*displacedLight.coords[1]-Math.sin(angle)*displacedLight.coords[2];

        displacedLight.coords[2]=Math.cos(angle)*displacedLight.coords[2]+Math.sin(angle)*yStore;

        for(int i=0;i<3;i++){
            if(i!=0){
                displacedLight.coords[i]=origin.coords[i]+displacedLight.coords[i];
                this.coords[i]=displacedLight.coords[i];
            }
        }
    }

    public static Vec subs(Vec v, Vec u){
        Vec newVec= new Vec();
        for (int i = 0; i < 3; i++) {
            newVec.coords[i]=v.coords[i]-u.coords[i];
        }
        return newVec;
    }

    public static Vec opposite(Vec v){
        return new Vec(-v.coords[0],-v.coords[1],-v.coords[2]);
    }

    public static Vec add(Vec v, Vec u){
        return new Vec(v.coords[0]+u.coords[0],v.coords[1]+u.coords[1],v.coords[2]+u.coords[2]);
    }

    public static void pointSpin(Vec axis, Vec center, Vec point, double angle){
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

    @Override
    public String toString(){
        return "{"+coords[0]+", "+coords[1]+", "+coords[2]+", "+coords[3]+"}";
    }

    @Override
    public boolean equals(Object v){
        if (this == v) return true;
        if (!(v instanceof Vec)) return false;
        Vec objVec=(Vec) v;
        return coords[0]==objVec.coords[0] && coords[1]==objVec.coords[1] && coords[2]==objVec.coords[2];
    }

    @Override
    public int hashCode() {
        int result = Double.hashCode(coords[0]);
        result = 31 * result + Double.hashCode(coords[1]);
        result = 31 * result + Double.hashCode(coords[2]);
        return result;
    }
}
