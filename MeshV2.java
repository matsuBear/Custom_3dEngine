
import java.util.HashMap;
import java.util.HashSet;

public class MeshV2 {
    Triangle[] tris;
    int color;
    Vec meshCenter;

    MeshV2 boundingBox;
    double[] depthList;
    double[] maxCoords;

    Triangle[] preloadedPolys;
    HashSet<Vec> vecList;

    HashSet<Vec> seenVecs;

    public MeshV2(int color, Triangle... polys) {
        this.color = color;
        tris = polys;
        preloadedPolys = new Triangle[tris.length];
        depthList = new double[tris.length];

        for (int i = 0; i < preloadedPolys.length; i++) {
            preloadedPolys[i] = new Triangle(new Vec(), new Vec(), new Vec());
        }

        vecList = new HashSet<>();

        for(Triangle t:tris){
            for(Vec v:t.vecs){
                if(!vecList.contains(v)){
                    vecList.add(v);
                }
            }
        }

        seenVecs = new HashSet<>(2*tris.length/3);
        movedVecs = new HashSet<>(2*tris.length/3);
    }

    public MeshV2(int color, int size) {

        tris = new Triangle[size];
        this.color = color;

        preloadedPolys = new Triangle[tris.length];
        depthList = new double[tris.length];

        for (int i = 0; i < preloadedPolys.length; i++) {
            preloadedPolys[i] = new Triangle(new Vec(), new Vec(), new Vec());
        }

        seenVecs = new HashSet<>(2*tris.length/3);
        movedVecs = new HashSet<>(2*tris.length/3);

    }

    public static MeshV2 icosahedronGen(double radius, Vec center, int color){
        double phi = (1 + Math.sqrt(5)) / 2;

        Vec[] vertices=new Vec[12];

        vertices[0]=new Vec( 0,  1,  phi);
        vertices[1]=new Vec( 0, -1,  phi);
        vertices[2]=new Vec( 0,  1, -phi);
        vertices[3]=new Vec( 0, -1, -phi);
        vertices[4]=new Vec( 1,  phi,  0);
        vertices[5]=new Vec(-1,  phi,  0);
        vertices[6]=new Vec( 1, -phi,  0);
        vertices[7]=new Vec(-1, -phi,  0);
        vertices[8]=new Vec( phi,  0,  1);
        vertices[9]=new Vec(-phi,  0,  1);
        vertices[10]=new Vec( phi,  0, -1);
        vertices[11]=new Vec(-phi,  0, -1);

        for (int i = 0; i < vertices.length; i++) {
            vertices[i] = Vec.normaliseVec(vertices[i]);
            vertices[i].coords[0] *= radius;
            vertices[i].coords[1] *= radius;
            vertices[i].coords[2] *= radius;
            vertices[i].coords[0] += center.coords[0];
            vertices[i].coords[1] += center.coords[1];
            vertices[i].coords[2] += center.coords[2];
        }
        int[][] faces = {
            {0, 1, 8},
            {0, 1, 9},
            {0, 4, 5},
            {0, 4, 8},
            {0, 5, 9},
            {1, 6, 7},
            {1, 6, 8},
            {1, 7, 9},
            {2, 3, 10},
            {2, 3, 11},
            {2, 4, 5},
            {2, 4, 10},
            {2, 5, 11},
            {3, 6, 7},
            {3, 6, 10},
            {3, 7, 11},
            {4, 8, 10},
            {5, 9, 11},
            {6, 8, 10},
            {7, 9, 11}
        };

        Triangle[] polygons=new Triangle[20];
        for(int i=0; i<faces.length;i++){
                polygons[i]=new Triangle(vertices[faces[i][0]],vertices[faces[i][1]],vertices[faces[i][2]]);
        }
        
        MeshV2 ico = new MeshV2(color, polygons);
        ico.meshCenter = new Vec(center.coords[0], center.coords[1], center.coords[2]);
        ico.correctWindingOrder(center);
        return ico;
    }

    public static Vec[] generateEvenFromCenter(double radius, Vec center, int number){
        Vec[] vecArray=new Vec[number];
        double constant=2*Math.PI/number;
        for(int i=0;i<number;i++){
            vecArray[i]=new Vec(
                center.coords[0]+radius*Math.cos(constant*i),
                center.coords[1],
                center.coords[2]+radius*Math.sin(constant*i)
            );
        }

        return vecArray;
    }

    public static MeshV2 icosphereGen(double radius, Vec center, int color, int subdivisions){
        return MeshV2.icosphereGenHelper(MeshV2.icosahedronGen(radius, center, color), radius, center, color, subdivisions, 0);
    }

    public static MeshV2 icosphereGenHelper(MeshV2 icosahedron, double radius, Vec center, int color, int subdivisions, int left){

        Triangle[] resultTris=new Triangle[icosahedron.tris.length*4];
        int index=0;

        HashMap<Integer, Vec> middlePointsMap=new HashMap<>();
        for(Triangle t:icosahedron.tris){
            Vec middle0;
            Vec middle1;
            Vec middle2;

            int a0 = System.identityHashCode(t.vecs[0]);
            int a1 = System.identityHashCode(t.vecs[1]);
            int a2 = System.identityHashCode(t.vecs[2]);
            int key0 = Math.min(a0,a1) * 100003 + Math.max(a0,a1);
            int key1 = Math.min(a0,a2) * 100003 + Math.max(a0,a2);
            int key2 = Math.min(a1,a2) * 100003 + Math.max(a1,a2);

            if(middlePointsMap.containsKey(key0)){
                middle0=middlePointsMap.get(key0);
            }else{
                middle0=Vec.add(t.vecs[1], t.vecs[0]);
                Vec.Div(middle0, 2);
                middle0 = Vec.subs(middle0, center);
                middle0=Vec.normaliseVec(middle0);
                middle0=Vec.mult(middle0, radius);
                middle0=Vec.add(center, middle0);
                middlePointsMap.put(key0, middle0);
            }

            if(middlePointsMap.containsKey(key1)){
                middle1=middlePointsMap.get(key1);
            }else{
                middle1=Vec.add(t.vecs[0], t.vecs[2]);
                Vec.Div(middle1, 2);
                middle1 = Vec.subs(middle1, center);
                middle1=Vec.normaliseVec(middle1);
                middle1=Vec.mult(middle1, radius);
                middle1=Vec.add(center, middle1);
                middlePointsMap.put(key1, middle1);
            }

            if(middlePointsMap.containsKey(key2)){
                middle2=middlePointsMap.get(key2);
            }else{
                middle2=Vec.add(t.vecs[1], t.vecs[2]);
                Vec.Div(middle2, 2);
                middle2 = Vec.subs(middle2, center);
                middle2=Vec.normaliseVec(middle2);
                middle2=Vec.mult(middle2, radius);
                middle2=Vec.add(center, middle2);
                middlePointsMap.put(key2, middle2);
            }
            
            resultTris[index]=new Triangle(t.vecs[0], middle0, middle1);
            index++;
            resultTris[index]=new Triangle(middle0, t.vecs[1], middle2)  ;
            index++;
            resultTris[index]=new Triangle(middle1, middle2, t.vecs[2]) ;
            index++;
            resultTris[index]=new Triangle(middle0, middle1, middle2);
            index++;
        }

        left++;

        MeshV2 newIcosahedron=new MeshV2(color, resultTris);
    
        newIcosahedron.meshCenter = new Vec(center.coords[0], center.coords[1], center.coords[2]);
        newIcosahedron.correctWindingOrder(newIcosahedron.meshCenter);

        if(subdivisions>left){
            return icosphereGenHelper(newIcosahedron, radius, center, color, subdivisions, left);
        }else{
            return newIcosahedron;
        }
    }

    public static MeshV2 heightMapGen(int x, int z, Formula yFormula, int color, Vec origin) {
        Vec[][] grid = Vec.heightMapVecGen(x, z, yFormula);
        MeshV2.centerHeightMap(x+(int)origin.coords[0], z+(int)origin.coords[2] , grid);
        MeshV2 heightMap = new MeshV2(color, (x - 1) * (z - 1) * 2);
        int trisIndex = 0;
        for (int i = 0; i < x - 1; i++) {
            for (int j = 0; j < z - 1; j++) {
                Vec topLeft = grid[i][j];
                Vec topRight = grid[i][j + 1];
                Vec bottomLeft = grid[i + 1][j];
                Vec bottomRight = grid[i + 1][j + 1];

                heightMap.tris[trisIndex] = new Triangle(topLeft, bottomLeft, bottomRight);
                trisIndex += 1;
                heightMap.tris[trisIndex] = new Triangle(topLeft, bottomRight, topRight);
                trisIndex += 1;
            }
        }

        heightMap.vecList=new HashSet<>();

        for(Triangle t:heightMap.tris){
            for(Vec v:t.vecs){
                if(!heightMap.vecList.contains(v)){
                    heightMap.vecList.add(v);
                }
            }
        }

        heightMap.meshCenter=new Vec(origin.coords[0],origin.coords[1],origin.coords[2]);
        //heightMap.maxCoords=Mesh.maxCoords(heightMap);
        //heightMap.boundingBox=cuboidGen(color, heightMap.maxCoords[0]*2, heightMap.maxCoords[1]*2, heightMap.maxCoords[2]*2, origin);
        return heightMap;
    }

    public static MeshV2 cuboidGen(int color, double width, double height, double depth, Vec origin) {
    double x = origin.coords[0] - width / 2;
    double y = origin.coords[1] - height / 2;
    double z = origin.coords[2] - depth / 2;

    Vec v0 = new Vec(x, y, z);                         
    Vec v1 = new Vec(x, y + height, z);                
    Vec v2 = new Vec(x + width, y + height, z);        
    Vec v3 = new Vec(x + width, y, z);                 
    Vec v4 = new Vec(x, y, z + depth);                 
    Vec v5 = new Vec(x, y + height, z + depth);        
    Vec v6 = new Vec(x + width, y + height, z + depth);
    Vec v7 = new Vec(x + width, y, z + depth);         

    MeshV2 cuboid = new MeshV2(color,
            new Triangle(v0, v1, v2), new Triangle(v0, v2, v3),
            new Triangle(v3, v2, v6), new Triangle(v3, v6, v7),
            new Triangle(v7, v6, v5), new Triangle(v7, v5, v4),
            new Triangle(v4, v5, v1), new Triangle(v4, v1, v0),
            new Triangle(v1, v5, v6), new Triangle(v1, v6, v2),
            new Triangle(v7, v4, v0), new Triangle(v7, v0, v3)
    );
    cuboid.correctWindingOrder(origin);
    cuboid.meshCenter=new Vec(origin.coords[0],origin.coords[1],origin.coords[2]);

    return cuboid;
}

    public static MeshV2 cubeGen(int color, double unit, Vec origin) {
        
        double x = origin.coords[0] - unit / 2;
        double y = origin.coords[1] - unit / 2;
        double z = origin.coords[2] - unit / 2;

        
        Vec v0 = new Vec(x, y, z);                 
        Vec v1 = new Vec(x, y + unit, z);          
        Vec v2 = new Vec(x + unit, y + unit, z);   
        Vec v3 = new Vec(x + unit, y, z);          
        Vec v4 = new Vec(x, y, z + unit);          
        Vec v5 = new Vec(x, y + unit, z + unit);   
        Vec v6 = new Vec(x + unit, y + unit, z + unit);
        Vec v7 = new Vec(x + unit, y, z + unit);

        MeshV2 cube = new MeshV2(color,
                new Triangle(v0, v1, v2),
                new Triangle(v0, v2, v3),
                new Triangle(v3, v2, v6),
                new Triangle(v3, v6, v7),
                new Triangle(v7, v6, v5),
                new Triangle(v7, v5, v4),
                new Triangle(v4, v5, v1),
                new Triangle(v4, v1, v0),
                new Triangle(v1, v5, v6),
                new Triangle(v1, v6, v2),
                new Triangle(v7, v4, v0),
                new Triangle(v7, v0, v3)
        );
        cube.correctWindingOrder(origin);
        cube.meshCenter=new Vec(origin.coords[0],origin.coords[1],origin.coords[2]);

        return cube;
    }

    public Vec findMeshCenter(){
        Vec center=new Vec();
        HashSet<Vec> seenVecs = new HashSet<>();
        for(Triangle t:tris){
            for(Vec v:t.vecs){
                if(!seenVecs.contains(v)){
                    for(int i=0;i<3;i++){
                        center.coords[i]+=v.coords[i];
                    }
                }
                seenVecs.add(v);
            }
        }
        Vec.Div(center, seenVecs.size());
        return center;
    }

    public void correctWindingOrder(Vec center){
        for(Triangle tri: tris){
            Vec triToCenter=Vec.subs(tri.vecs[0], center);
            
            double dot = Vec.scalarProduct(tri.normal, triToCenter);
        
            if (dot < 0) {
                Vec temp = tri.vecs[1];
                tri.vecs[1] = tri.vecs[2];
                tri.vecs[2] = temp;
                tri.triangleNormal();
                tri.centroidCalc();
            }
        }
    }

    public static void centerHeightMap(int x, int z, Vec[][] grid) {
        for (Vec[] vecs : grid) {
            for (Vec v : vecs) {
                v.coords[0] -= (double) (x - 1) / 2;
                v.coords[2] -= (double) (z - 1) / 2;
            }
        }
    }

    public int cameraCalcs(Camera camera, Boolean backCulling) {

        int drawIndex = 0;
        if(backCulling){
            for (Triangle triangle : tris) {
                
                preloadedPolys[drawIndex].triangleCameraSpace(triangle, camera, camera.target);
            
                if (Vec.scalarProduct(preloadedPolys[drawIndex].normal, preloadedPolys[drawIndex].vecs[0])<0){
                    if(preloadedPolys[drawIndex].centroid.coords[2]>0.1){
                        drawIndex += 1;
                    }
                }

            }
        }else{

            for (Triangle triangle : tris) {

                preloadedPolys[drawIndex].triangleCameraSpace(triangle, camera, camera.target);
                if(triangle.centroid.coords[2]>=camera.origin.coords[2]+0.1){
                    drawIndex += 1;
                }
            }
        }

        /*for (int i = 0; i < drawIndex; i++) {
            depthList[i] = preloadedPolys[i].depth();
        }
        Sort.insertion(depthList, preloadedPolys, drawIndex);*/

        return drawIndex;
    }

    public void drawPolygons(Renderer r, int[] pixels, double[] zBuffer, int drawIndex, Matrix m, double width, double height, Camera camera,
            Vec light, Boolean fill, Boolean lightSource) {
        
        Vec ligthInCam = Matrix.multNew(light, camera.viewMatrix);

        if (ligthInCam.coords[3] != 0 && ligthInCam.coords[3] != 1) {
            Vec.Div(ligthInCam, ligthInCam.coords[3]);
        }

        for (int i = 0; i < drawIndex; i++) {
            if (preloadedPolys[i] == null) {
                break;
            } else {

                int polyColor;

                if(!lightSource){
                    Vec lightDir=(Vec.subs(ligthInCam, preloadedPolys[i].centroid));
                    double lightDistance=Vec.size(lightDir);
                    double attenuation=1/(1+0.00001*lightDistance+0.0005*lightDistance*lightDistance);

                    double lightScalar = Vec.scalarProduct(Vec.normaliseVec(lightDir), preloadedPolys[i].normal)*attenuation;
                    
                    int red = ((color >> 16) & 0xFF);
                    int green = ((color >> 8)  & 0xFF);
                    int blue = (color         & 0xFF);
                    float t = (float)Math.max(lightScalar, 0.04f);
                    polyColor = ((int)(red * t) << 16) | ((int)(green * t) << 8) | (int)(blue * t);

                }else{

                    polyColor=color;

                }

                preloadedPolys[i].triangleTranslate(m);
                preloadedPolys[i].triangleProject(width, height);
                
                if (fill) r.fillTriangle(preloadedPolys[i].vecs[0], preloadedPolys[i].vecs[1], preloadedPolys[i].vecs[2], pixels, zBuffer, polyColor);
                else r.drawTriangle(preloadedPolys[i].vecs[0], preloadedPolys[i].vecs[1], preloadedPolys[i].vecs[2], pixels, zBuffer, polyColor);
            }
        }
    }


    public void drawMesh(Renderer r, int[] pixels, double[] zBuffer, Matrix m, double width, double height, Camera camera,
         Vec light, Boolean backCulling, Boolean fill, Boolean lightSource, Boolean pointCloud){

        int drawIndex=cameraCalcs(camera, backCulling);
        if(!pointCloud){
            drawPolygons(r, pixels, zBuffer, drawIndex, m, width, height, camera, light, fill, lightSource);
        }
    }

    HashSet<Vec> cloudVecs=new HashSet<>();

    public void drawPointCloud(Renderer r, int[] pixels, double[] zBuffer, int drawIndex, Matrix m, double width, double height,
        Camera camera, Vec light, Boolean fill, Boolean lightSource){

            Vec ligthInCam = Matrix.multNew(light, camera.viewMatrix);

        if (ligthInCam.coords[3] != 0 && ligthInCam.coords[3] != 1) {
            Vec.Div(ligthInCam, ligthInCam.coords[3]);
        }

        for (int i = 0; i < drawIndex; i++) {
            if (preloadedPolys[i] == null) {
                break;
            } else {

                int polyColor;

                if(!lightSource){
                    Vec lightDir=(Vec.subs(ligthInCam, preloadedPolys[i].centroid));
                    double lightDistance=Vec.size(lightDir);
                    double attenuation=1/(1+0.00001*lightDistance+0.0005*lightDistance*lightDistance);

                    double lightScalar = Vec.scalarProduct(Vec.normaliseVec(lightDir), preloadedPolys[i].normal)*attenuation;
                    
                    int red = ((color >> 16) & 0xFF);
                    int green = ((color >> 8)  & 0xFF);
                    int blue = (color         & 0xFF);
                    float t = (float)Math.max(lightScalar, 0.04f);
                    polyColor = ((int)(red * t) << 16) | ((int)(green * t) << 8) | (int)(blue * t);

                }else{

                    polyColor=color;

                }

                preloadedPolys[i].triangleTranslate(m);
                preloadedPolys[i].triangleProject(width, height);

                for(Vec v:preloadedPolys[i].vecs){
                    if(!cloudVecs.contains(v)){
                        r.drawBillboardPoint(v, pixels, zBuffer, width, polyColor);
                        cloudVecs.add(v);
                    }
                }
            }
        }
    }

    public void rotate(Vec axis, double angle) {
        seenVecs.clear();
        for (Triangle tri : tris) {
            for (Vec v:tri.vecs){
                if(!seenVecs.contains(v)){
                    Vec.pointSpin(axis, meshCenter, v, angle);
                    seenVecs.add(v);
                }
            }
            tri.triangleNormal();
            tri.centroidCalc();
        }
    }

    public void heightMapUpdate(Formula f, double time) {

        for (Triangle t : tris) {

            for (Vec v : t.vecs) {

                if(!v.seen){
                    v.coords[1]=f.calc(v.coords[0]+time, v.coords[2]+time);
                    v.seen=true;
                }

            }

            t.triangleNormal();
            t.centroidCalc();

        }

        for(Vec v:vecList){
            v.seen=false;
        }
    }

    public static double[] maxCoords(MeshV2 m){

        double[] temp;
        double maximumX=-1000000;
        double maximumY=-1000000;
        double maximumZ=-1000000;
        for(Triangle t:m.tris){
            temp=Triangle.maxCoords(t);
                maximumX=(maximumX<=temp[0])? temp[0]:maximumX;
                maximumY=(maximumY<=temp[1])? temp[1]:maximumY;
                maximumZ=(maximumZ<=temp[2])? temp[2]:maximumZ;
        }

        return new double[]{maximumX,maximumY,maximumZ};
    }

    HashSet<Vec> movedVecs;

    public void move(Vec displacement){
        movedVecs.clear();
        for(Triangle t:tris){
            for(Vec v:t.vecs){
                if(!movedVecs.contains(v)){
                    for(int i=0;i<3;i++){
                        v.coords[i]+=displacement.coords[i]-meshCenter.coords[i];
                    }
                    movedVecs.add(v);
                }
            }
            t.triangleNormal();
            t.centroidCalc();
        }

        for(int i=0;i<3;i++){
            meshCenter.coords[i]=displacement.coords[i];
        }

    }
}
