template <typename T>
void diff_of_arrays(T arr1[], T arr2[], T diff[], int size, T init = 0) {
    for (int i = 0; i < size; ++i) {
        diff[i] = arr1[i] - arr2[i] + init;
    }
}