
import java.util.Random;

public class Sort {
    public static void main(String[] args) {
        Random r=new Random();
        int[] tab=new int[1000000];
        for(int index=0; index<tab.length; index++){
            tab[index]=r.nextInt(10000000);
        }
        separate(tab);
        viewArray(tab);
    }

    private static void viewArray(int arr[]){
        for(int value:arr){
            System.out.println(value);
        }
        System.out.println("");
    }

    private static void separate(int[] arr) {
        if(arr.length<2){
            return;
        }
        else {
            int middle=arr.length/2;
            int[] half_1=new int[middle];
            int[] half_2=new int[arr.length-middle];
            for (int index=0; index<arr.length; index++){
                if(index<middle){
                    half_1[index]=arr[index];
                }
                else{
                    half_2[index-middle]=arr[index];
                }
            }
            separate(half_1);
            separate(half_2);
            conquer(arr, half_1, half_2);
        }
    }

    private static void conquer(int[] arr, int[] left, int[] right){
        int index_arr=0, index_left=0, index_right=0;
        while (index_left<left.length && index_right<right.length) {
            if(left[index_left]<right[index_right]){
                arr[index_arr]=left[index_left];
                index_arr++;
                index_left++;
            }else{
                arr[index_arr]=right[index_right];
                index_arr++;
                index_right++;
            }
        }
        while(index_right<right.length){
            arr[index_arr]=right[index_right];
            index_arr++;
            index_right++;
        }
        while(index_left<left.length){
            arr[index_arr]=left[index_left];
            index_arr++;
            index_left++;
        }
    }

    private static void viewArray(double arr[]) {
        for(double value : arr) {
            System.out.println(value);
        }
        System.out.println("");
    }

    private static void separate(double[] arr) {
        if(arr.length < 2) {
            return;
        }
        int middle = arr.length / 2;
        double[] half_1 = new double[middle];
        double[] half_2 = new double[arr.length - middle];
        for (int index = 0; index < arr.length; index++) {
            if(index < middle) {
                half_1[index] = arr[index];
            } else {
                half_2[index - middle] = arr[index];
            }
        }
        separate(half_1);
        separate(half_2);
        conquer(arr, half_1, half_2);
    }

    private static void conquer(double[] arr, double[] left, double[] right) {
        int index_arr = 0, index_left = 0, index_right = 0;
        while (index_left < left.length && index_right < right.length) {
            if(left[index_left] < right[index_right]) {
                arr[index_arr] = left[index_left];
                index_arr++;
                index_left++;
            } else {
                arr[index_arr] = right[index_right];
                index_arr++;
                index_right++;
            }
        }
        while(index_right < right.length) {
            arr[index_arr] = right[index_right];
            index_arr++;
            index_right++;
        }
        while(index_left < left.length) {
            arr[index_arr] = left[index_left];
            index_arr++;
            index_left++;
        }
    }

    public static void separate(double[] arr, Triangle[] tris) {
        if(arr.length < 2) {
            return;
        }
        int middle = arr.length / 2;
        double[] half_1 = new double[middle];
        double[] half_2 = new double[arr.length - middle];
        Triangle[] t_half_1 = new Triangle[middle];
        Triangle[] t_half_2 = new Triangle[arr.length - middle];
        for (int index = 0; index < arr.length; index++) {
            if(index < middle) {
                half_1[index] = arr[index];
                t_half_1[index]=tris[index];
            } else {
                half_2[index - middle] = arr[index];
                t_half_2[index-middle]= tris[index];
            }
        }
        separate(half_1,t_half_1);
        separate(half_2, t_half_2);
        conquer(arr, half_1, half_2, tris, t_half_1, t_half_2);
    }

    public static void conquer(double[] arr, double[] left, double[] right, Triangle[] t_arr, Triangle[] t_left, Triangle[] t_right){
        int index_arr = 0, index_left = 0, index_right = 0;
        while (index_left < left.length && index_right < right.length) {
            if(left[index_left] > right[index_right]) {
                arr[index_arr] = left[index_left];
                t_arr[index_arr]=t_left[index_left];
                index_arr++;
                index_left++;
            } else {
                arr[index_arr] = right[index_right];
                t_arr[index_arr]=t_right[index_right];
                index_arr++;
                index_right++;
            }
        }
        while(index_right < right.length) {
            arr[index_arr] = right[index_right];
            t_arr[index_arr]=t_right[index_right];
            index_arr++;
            index_right++;
        }
        while(index_left < left.length) {
            arr[index_arr] = left[index_left];
            t_arr[index_arr]=t_left[index_left];
            index_arr++;
            index_left++;
        }
    }

    public static void triDepthSort(double[] arr, Triangle[] tris){
        separate(arr, tris);
    }

    public static void insertion(double[] arr, Triangle[] tris, int index){
        for(int i=0; i<index-1;i++){
            if(arr[i]>arr[i+1]){
                int back=i;
                double temp=arr[back];
                Triangle tTemp=tris[back];
                while(back>0 && arr[back]<arr[back-1]){
                    arr[back]=arr[back-1];

                    tris[back]=tris[back-1];
                    back--;
                }
                arr[back]=temp;
                tris[back]=tTemp;
            }
        }
    }   
}

