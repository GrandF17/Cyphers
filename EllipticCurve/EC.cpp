#include <openssl/obj_mac.h>
#include <openssl/ec.h>
#include <iostream>
#include <cmath>
using namespace std;

// void *addPoint(void *arg)
// {
//     EC_POINT_add(group, new_point, new_point, points[0], ctx);

//     pthread_exit(NULL);
// }

// gcc -g -Wall -Wextra -o EC EC.cpp -lcrypto -lstdc++

int main(void)
{
    EC_GROUP *group = EC_GROUP_new_by_curve_name(NID_secp112r1);
    EC_POINT *point = EC_POINT_new(group);

    BN_CTX *ctx = BN_CTX_new();
    BIGNUM *x = BN_new();
    BIGNUM *y = BN_new();
    BN_hex2bn(&x, "09487239995A5EE76B55F9C2F098");
    BN_hex2bn(&y, "A89CE5AF8724C0A23E0E0FF77500");
    EC_POINT_set_affine_coordinates_GFp(group, point, x, y, ctx);

    cout << "kek" << endl;
    long long max_iterations = (long long)pow(2, 32); // cant use 57 bits :(
    cout << "kek" << endl;
    long long iteration_count = 0;

    cout << "kek" << endl;
    EC_POINT **points = new EC_POINT *[max_iterations];
    cout << "kek" << endl;
    points[0] = EC_POINT_dup(point, group);
    cout << "kek" << endl;

    cout << "kek" << endl;
    while (iteration_count < max_iterations)
    {
        // Дублируем текущую точку
        EC_POINT *new_point = EC_POINT_dup(point, group);

        cout << new_point << endl;

        // Складываем текущую точку с начальной точкой
        EC_POINT_add(group, new_point, new_point, points[0], ctx);

        // Сравниваем новую точку с предыдущими точками
        for (int i = 1; i <= iteration_count; i++)
        {
            if (EC_POINT_cmp(group, new_point, points[i], ctx) == 0)
            {
                // Найдена периодическая последовательность
                std::cout << "Period found: " << i << std::endl;
                return 0;
            }
        }

        // Добавляем новую точку в массив
        iteration_count++;
        points[iteration_count] = new_point;

        // Переходим к следующей итерации
        point = new_point;
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