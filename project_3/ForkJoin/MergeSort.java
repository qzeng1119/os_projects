// suppose that:
// the input data are integers, separated by *space* and end with '\n'

import java.util.Scanner;
import java.util.concurrent.RecursiveAction;
import java.util.concurrent.ForkJoinPool;

public class MergeSort extends RecursiveAction {

    private static final int THRESHOLD = 100;
    private int[] arr;
    private int left;
    private int right;

    public MergeSort(int[] arr, int left, int right) {
        this.arr = arr;
        this.left = left;
        this.right = right;
    }

    @Override
    protected void compute() {
        // small enough
        if (right - left + 1 <= THRESHOLD) {
            insertionSort(arr, left, right);
            return;
        }

        // large, fork 
        int mid = (left + right) / 2;
        MergeSort leftTask = new MergeSort(arr, left, mid);
        MergeSort rightTask = new MergeSort(arr, mid + 1, right);

        invokeAll(leftTask, rightTask);
        merge(arr, left, mid, right);
    }

    private void merge(int[] arr, int left, int mid, int right) {
        int n1 = mid - left + 1;
        int n2 = right - mid;

        int[] L = new int[n1];
        int[] R = new int[n2];

        System.arraycopy(arr, left, L, 0, n1);
        System.arraycopy(arr, mid + 1, R, 0, n2);

        int i = 0, j = 0, k = left;

        while (i < n1 && j < n2) {
            if (L[i] <= R[j]) {
                arr[k++] = L[i++];
            } else {
                arr[k++] = R[j++];
            }
        }

        while (i < n1) arr[k++] = L[i++];
        while (j < n2) arr[k++] = R[j++];
    }

    private void insertionSort(int[] arr, int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            int key = arr[i];
            int j = i - 1;

            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        String line = scanner.nextLine();        
        String[] parts = line.split(" ");        

        // convert to int
        int[] arr = new int[parts.length];
        for (int i = 0; i < parts.length; i++) {
            arr[i] = Integer.parseInt(parts[i]);
        }

        // sort in parallel
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(new MergeSort(arr, 0, arr.length - 1));
        pool.shutdown();

        // print the sorted data
        System.out.print("The sorted data: \n");
        for (int num : arr) System.out.print(num + " ");
        System.out.print('\n');

        scanner.close();
    }
}