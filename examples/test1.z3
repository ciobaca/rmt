(declare-datatypes () ((Var varX varY varZ varC1 varC2 varV1 varV2)))

(declare-const ENV (Array Var Int))
(declare-const SUM Int)
(declare-const I Int)
(declare-const N Int)

(define-fun conjecture () Bool

(and (and (and (<= 0 N) (<= (select ENV varY) N)) (<= (select ENV varY) N)) (not (and (and (and (<= 0 N)

(<= (select ENV varY) N)) (<= (select ENV varY) N)) (and (<= 0 N) (<= (select (store (store ENV varZ

(+ (select ENV varZ) (select ENV varY))) varY (+ (select (store ENV varZ (+ (select ENV varZ) (select ENV varY)))

varY) 1)) varY) N))))

))

(simplify conjecture)
(assert (not conjecture))
(check-sat)
(get-model)
