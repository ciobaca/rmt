;;(set-logic ALL_SUPPORTED)
(declare-fun gcd (Int Int) Int)
(assert (forall ((x Int) (y Int))
	(= (gcd x y)
           (ite (= y 0)
              x
              (gcd y (mod x y))
           ))))
(declare-const u Int)
(declare-const v Int)
(declare-const x Int)
(declare-const y Int)
(declare-const a Int)
(declare-const b Int)
(declare-const c Int)
(declare-const d Int)

(assert (= (gcd u v) (gcd x y)))
(assert (not (= y 0)))
(assert (= x (+ (* a u) (* b v))))
(assert (= (+ (* c u) (* d v)) y))
(assert (not (= (- x (* y (div x y)))
                (-
		x ;;(+ (* a u) (* b v))
		(* (div x y) (+ (* c u) (* d v)))
		))))

(check-sat)
