(define-fun-rec gcd ((x Int) (y Int)) Int
     (ite (= y 0)
      x
      (gcd y (mod x y))
     )
)
(declare-const X Int)
(declare-const Y Int)
(declare-const A Int)
(declare-const B Int)
(declare-const C Int)
(declare-const D Int)
(declare-const I Int)
(declare-const J Int)
(declare-const I_11 Int)
(declare-const J_11 Int)
(assert
(and 
     (and 
     (and 
     (= X (+ (* A I) (* B J))) 
     (= Y (+ (* C I) (* D J))))
     (= Y 0))
     (not (and (and (and (and (and 
	      (= X (+ (* A I) (* B J))) 
	      (= Y (+ (* C I) (* D J))))
	      (= Y 0)) 
	      (= (gcd I J) (+ (* A I) (* B J))))))))
)
(check-sat)
(get-model)
