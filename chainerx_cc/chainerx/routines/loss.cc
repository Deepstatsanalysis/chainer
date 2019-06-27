#include "chainerx/routines/loss.h"

#include <string>
#include <vector>
#include "chainerx/array.h"
#include "chainerx/routines/arithmetic.h"
#include "chainerx/routines/creation.h"
#include "chainerx/routines/explog.h"
#include "chainerx/routines/indexing.h"
#include "chainerx/routines/manipulation.h"
#include "chainerx/routines/misc.h"
#include "chainerx/scalar.h"

namespace chainerx {

Array AbsoluteError(const Array& x1, const Array& x2) { return Absolute(x1 - x2); }

Array SquaredError(const Array& x1, const Array& x2) { return SquaredDifference(x1, x2); }

Array GaussianKLDivergence(const Array& mean, const Array& ln_var) { return (Square(mean) + Exp(ln_var) - ln_var - 1) * 0.5; }

Array HuberLoss(const Array& x1, const Array& x2, Scalar delta) {
    Array a = x1 - x2;
    Array abs_a = Absolute(a);
    Array delta_array = chainerx::FullLike(a, delta, a.device());

    // TODO(kshitij12345) : use Array < Scalar when implemented.
    return Where(abs_a < delta_array, 0.5 * Square(a), delta * (abs_a - Scalar{0.5} * delta));
}

Array Hinge(const Array& x1, const Array& x2, const std::string& norm) {
    int64_t num = x1.shape()[0];
    Array bottom_diff = Copy(x1);
    std::vector<Array> indices = {Arange(num), x2};
    Array concat = Transpose(Concatenate(indices, 0));
    IMultiply(At(bottom_diff, concat), -1);
    bottom_diff = Maximum(0, Add(1, bottom_diff));

    if (norm == "L1") {
        return bottom_diff;
    } else {
        return Square(bottom_diff);
    }
}

}  // namespace chainerx
