#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(void)
{
    // Total number of processes running.
    int common_sz;

    // Number of my process. Consider 0 as the root.
    int my_rank;

    // Start of the range.
    float a;

    // End of the range.
    float b;

    // Length of the random number sequence.
    int data_count;

    // Number of bins.
    int bin_count;

    // Selects a "random" seed for the rand engine.
    srand((unsigned int)time(NULL));

    // Initialize MPI with no argc nor argv.
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &common_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        printf("Type in the values for: <a> <b> <data_count> <bin_count>: ");
        fflush(stdout);
        scanf("%f %f %d %d", &a, &b, &data_count, &bin_count);
    }

    // Broadcast the input values to all processes.
    MPI_Bcast(&a, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&b, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&data_count, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&bin_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

    // Interval between each bin.
    float interval = (b - a) / (float)bin_count;

    // Makes data_count a multiple of common_sz.
    int local_data_count = data_count / common_sz;
    data_count = local_data_count * common_sz;

    // Holds a chunk of data.
    float *local_data = malloc(local_data_count * sizeof(*local_data));

    // Holds the sequence of the generated random numbers in the range [a, b].
    float *data;

    if (my_rank == 0)
    {
        // Generate the sequence of random numbers in ther range [a, b].
        data = malloc(data_count * sizeof(*data));
        for (int i = 0; i < data_count; ++i)
        {
            data[i] = (float)rand() / (float)RAND_MAX * (b - a) + a;
        }
    }

    // Sends a data chunk of length `local_data_count` to all processes.
    MPI_Scatter(data, local_data_count, MPI_FLOAT, local_data, local_data_count,
                MPI_FLOAT, 0, MPI_COMM_WORLD);

    // Frees data as it has been sent to all processes.
    if (my_rank == 0) {
        free(data);
    }

    // Holds the local histogram.
    int *local_histogram = malloc(bin_count * sizeof(*local_histogram));
    memset(local_histogram, 0, bin_count * sizeof(*local_histogram));

    // Computes the histogram of this data chunk.
    for (int i = 0; i < local_data_count; ++i)
    {
        int bin_index = (local_data[i] - a) / interval;
        local_histogram[bin_index] += 1;
    }

    // Holds the sum of all local histograms.
    int *histogram = malloc(bin_count * sizeof(*histogram));

    // Computes the sum of all local histograms.
    MPI_Reduce(local_histogram, histogram, bin_count, MPI_INT, MPI_SUM, 0,
               MPI_COMM_WORLD);

    if (my_rank == 0)
    {
        int max_bin_count = 0;

        // Get the maximum bin counting value.
        for (int i = 0; i < bin_count; ++i)
        {
            if (histogram[i] > max_bin_count)
                max_bin_count = histogram[i];
        }

        // Pretty prints the histogram vector.
        printf(
            "sequence range: [%.2f, %.2f]\nsequence length: %d\nnumber of "
            "bins: %d\n"
            "interval: %.2f\n",
            a, b, data_count, bin_count, interval);
        for (int i = 0; i < bin_count; ++i)
        {
            float bin_index = (float)i * interval + a;
            printf("[%.2f, %.2f) |", bin_index, bin_index + interval);
            int row_width = ((float)histogram[i] / (float)max_bin_count) * 40.f;
            for (int j = 0; j < row_width; ++j)
            {
                printf("#");
            }
            printf(" %d\n", histogram[i]);
        }
        fflush(stdout);
    }

    // Looking for freeeeeeedoooooooooom!!!
    free(local_data);
    free(local_histogram);
    free(histogram);

    MPI_Finalize();
    return 0;
}
