#include <iostream>
#include <cmath>
#include <mpi.h>

// Fonksiyon tanýmý: Örneðin, 1 / (1 + x^2) fonksiyonu
double f(double x) {
    return 1.0 / (1.0 + x * x);
}

// Trapezoid kuralýný kullanarak parça alanýný hesapla
double compute_partial_area(double a, double b, int n) {
    double h = (b - a) / n;
    double partial_area = 0.0;
    for (int i = 1; i < n; ++i) {
        double x = a + i * h;
        partial_area += f(x);
    }
    partial_area += (f(a) + f(b)) / 2.0;
    partial_area *= h;
    return partial_area;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    // Integralin hesaplanacaðý aralýk ve bölme sayýsý
    double a = 0.0;  // Alt sýnýr
    double b = 10.0;  // Üst sýnýr
    int n = 1000000; // Bölme sayýsý

    // Her iþlemciye atanan parça aralýðýný hesapla
    int chunk_size = n / size;
    int start_index = rank * chunk_size;
    int end_index = (rank + 1) * chunk_size;

    // Her iþlemci kendi parça alanýný hesapla
    double partial_area = compute_partial_area(a + start_index * (b - a) / n, a + end_index * (b - a) / n, chunk_size);

    // Her iþlemcinin hesapladýðý parça alanlarýný topla
    double total_area = 0.0;
    MPI_Reduce(&partial_area, &total_area, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    // Her iþlemcinin hesapladýðý alaný yazdýr
    std::cout << "Process " << rank << " hesapladýðý alan: " << partial_area << std::endl;

    // Ana iþlemci toplam alaný yazdýrýr
    if (rank == 0) {
        std::cout << "Integral sonucu: " << total_area << std::endl;
    }

    MPI_Finalize();
    return 0;
}
