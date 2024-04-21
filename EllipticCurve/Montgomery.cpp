#include <gmpxx.h>

#include <iostream>
#include <tuple>

using namespace std;

class MontgomeryCurve {
   private:
    mpz_class a, b, r, rr, n;

   public:
    mpz_class p;

    MontgomeryCurve(mpz_class a_, mpz_class b_, mpz_class p_)
        : a(a_), b(b_), p(p_) {
        r = 1;
        for (int i = 0; i < p.get_mpz_t()->_mp_size; i++) {
            r <<= GMP_NUMB_BITS;
        }
        rr = r * r % p;
        n = (p + 1 - a) * r % p;
    }

    mpz_class add(mpz_class x1, mpz_class y1, mpz_class x2, mpz_class y2) {
        mpz_class x3, y3;
        if (x1 == x2 && y1 == y2) {
            mpz_class t = x1 * y1 % p;
            mpz_class u = (x1 + y1) % p;
            mpz_class v = (t + b) * inv(u, p) % p;
            x3 = (v * v - a - 2 * x1) % p;
            y3 = (v * (x1 - x3) - y1) % p;
        } else {
            mpz_class u = (y2 - y1) * inv(x2 - x1, p) % p;
            x3 = (u * u - a - x1 - x2) % p;
            y3 = (u * (x1 - x3) - y1) % p;
        }
        return toMontgomery(x3, y3);
    }

    mpz_class mul(mpz_class k, mpz_class x1, mpz_class y1) {
        mpz_class x2 = x1, y2 = y1;
        k = fromMontgomery(k, 0);
        mpz_class resx = 0, resy = 1;
        while (k > 0) {
            if (k % 2 == 1) {
                (resx, resy) = add(resx, resy, x2, y2);
            }
            (x2, y2) = add(x2, y2, x2, y2);
            k /= 2;
        }
        return toMontgomery(resx, resy);
    }

   private:
    mpz_class inv(mpz_class x, mpz_class y) {
        mpz_class s = 0, t = 1, r = y, r0 = x, q;
        while (r0 != 0) {
            q = r / r0;
            tie(s, t) = make_tuple(t, s - q * t);
            tie(r, r0) = make_tuple(r0, r - q * r0);
        }
        if (s < 0) {
            s += y;
        }
        return s;
    }

    mpz_class toMontgomery(mpz_class x, mpz_class y) { return x * r % p; }

    mpz_class fromMontgomery(mpz_class x, mpz_class y) {
        mpz_class u = (x * n % p) * rr % p;
        return u >= p ? u - p : u;
    }
};

int main() {
    MontgomeryCurve curve(9, 7,
                          mpz_class("578960446186580977117854925043439539266349"
                                    "92332820282019728792003956564819949"));
    mpz_class x = mpz_class("2");
    mpz_class y = mpz_class("2");
    cout << "Curve: y^2 = x^3 + 9x + 7 (mod p)" << endl;
    cout << "Prime p: " << curve.p << endl;
    cout << "Base point: (" << x << ", " << y << ")" << endl;
    cout << "Order of the base point: " << curve.mul(curve.p - 1, x, y) << endl;
    return 0;
}