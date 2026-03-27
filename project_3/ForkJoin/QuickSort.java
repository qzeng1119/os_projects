// suppose that:
// the input data are integers, separated by *space* and end with '\n'

import java.util.concurrent.RecursiveAction;
import java.util.Scanner;
import java.util.concurrent.ForkJoinPool;

public class QuickSort extends RecursiveAction {

    private static final int THRESHOLD = 100;
    private int[] arr;
    private int left;
    private int right;

    public QuickSort(int[] arr, int left, int right) {
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
        int pivotIndex = partition(arr, left, right);
        QuickSort leftTask = new QuickSort(arr, left, pivotIndex - 1);
        QuickSort rightTask = new QuickSort(arr, pivotIndex + 1, right);

        invokeAll(leftTask, rightTask);
    }

    private int partition(int[] arr, int left, int right) {
        int pivot = arr[left];
        int l = left, r = right;
        
        if (l >= r) return l;
        
        while (l < r) {
            while (l < r && arr[r] >= pivot) r--;
            arr[l] = arr[r];

            while (l < r && arr[l] <= pivot) l++;
            arr[r] = arr[l];
        }
        arr[l] = pivot;

        return l;
    }

    private void insertionSort(int[] arr, int left, int right) {
        for (int i = left + 1; i <= right; i++) {
            int key = arr[i];
            int j = i-1;

            while (j >= left && arr[j] > key) {
                arr[j + 1] = arr[j];
                j--;
            }
            arr[j + 1] = key;
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);

        String line = scanner.nextLine();        // end: '\n'
        String[] parts = line.split(" ");        // split sign: ' '

        // convert to int
        int[] arr = new int[parts.length];
        for (int i = 0; i < parts.length; i++) {
            arr[i] = Integer.parseInt(parts[i]);
        }

        // sort in parallel
        ForkJoinPool pool = new ForkJoinPool();
        pool.invoke(new QuickSort(arr, 0, arr.length - 1));
        pool.shutdown();

        // print the sorted data
        System.out.print("The sorted data: \n");
        for (int num : arr) System.out.print(num + " ");
        System.out.print('\n');

        scanner.close();
    }
}