#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <thread>
#include <vector>
#include <iostream>
#include <cmath>
using namespace std;

// COMPILATION:
// gcc -g -Wall -Wextra -o EC EC.cpp -lcrypto -lstdc++

// -_-_-_ behaviaral constants
const bool USE_THREADS = false;
const bool SHOW_POINTS = false;
const bool SHOW_COUNTER = true;

// -_-_-_ global vars -_-_-_
EC_GROUP *group;
BN_CTX *ctx;
EC_POINT *point;
EC_POINT **points;
EC_POINT *new_point;

long long iteration_count = 0;

void *addPoint(void *arg)
{
    // Дублируем текущую точку
    new_point = EC_POINT_dup(point, group);

    // Складываем текущую точку с начальной точкой
    EC_POINT_add(group, new_point, new_point, points[0], ctx);

    // Добавляем новую точку в массив
    iteration_count++;
    points[iteration_count] = new_point;

    // Переходим к следующей итерации
    point = new_point;
}

int main(void)
{
    group = EC_GROUP_new_by_curve_name(NID_secp112r1);
    point = EC_POINT_new(group);

    ctx = BN_CTX_new();
    BIGNUM *x = BN_new();
    BIGNUM *y = BN_new();
    BN_hex2bn(&x, "09487239995A5EE76B55F9C2F098");
    BN_hex2bn(&y, "A89CE5AF8724C0A23E0E0FF77500");
    EC_POINT_set_affine_coordinates_GFp(group, point, x, y, ctx);

    long long max_iterations = (long long)pow(2, 32); // cant use 57 bits :(

    points = new EC_POINT *[max_iterations];
    points[0] = EC_POINT_dup(point, group);

    long long compNum = (long long)pow(2, 31);

    while (iteration_count < max_iterations)
    {
        if (iteration_count <= compNum && USE_THREADS)
        {
            int limit = (int)pow(2, 8); // eq 4096 threads per operation

            pthread_t threads[limit];
            for (int i = 0; i < limit; i++)
            {
                int *p;
                int rc = pthread_create(&threads[i], NULL, addPoint, (void *)(p));

                if (rc)
                {
                    cout << "Error:unable to create thread," << rc << endl;
                    exit(-1);
                }
            }
            for (int i = 0; i < limit; i++)
                pthread_join(threads[i], NULL);

            cout << iteration_count << endl;
        }
        else
        {
            // Дублируем текущую точку
            new_point = EC_POINT_dup(point, group);

            // Складываем текущую точку с начальной точкой
            EC_POINT_add(group, new_point, new_point, points[0], ctx);

            // Сравниваем новую точку с предыдущими точками
            for (int i = 1; i <= iteration_count; i++)
            {
                if (EC_POINT_cmp(group, new_point, points[i], ctx) == 0)
                {
                    // Найдена периодическая последовательность
                    cout << "Period found: " << i << endl;
                    return 0;
                }
            }

            if (SHOW_POINTS)
                cout << new_point << endl;
            if (SHOW_COUNTER && iteration_count % 1024 == 0)
                cout << iteration_count << endl;

            // Добавляем новую точку в массив
            iteration_count++;
            points[iteration_count] = new_point;

            // Переходим к следующей итерации
            point = new_point;
        }
    }

    EC_POINT_free(point);
    for (int i = 0; i <= iteration_count; i++)
    {
        EC_POINT_free(points[i]);
    }
    delete[] points;
    BN_free(x);
    BN_free(y);
    BN_CTX_free(ctx);
    EC_GROUP_free(group);

    return 0;
}