;; fot-mode-el -- Major mode for FOT files

;; Author: Stefan Ciobaca
;; (C) 2016 Stefan Ciobaca stefan.ciobaca@gmail.com

(defvar fot-mode-hook nil)

(defvar fot-mode-map
  (let ((fot-mode-map (make-keymap)))
    (define-key fot-mode-map "\C-j" 'newline-and-indent)
    fot-mode-map)
  "Keymap for FOT major mode")
(add-to-list 'auto-mode-alist '("\\.fot\\'" . fot-mode))

(setq fot-keywords '("sorts" "subsort" "signature" "variables" "rewrite-system" "smt-narrow-search" "smt-unify" "smt-implies" "smt-satisfiability" "smt-prove"))
(setq fot-keywords-regexp (regexp-opt fot-keywords 'words))

(setq fot-types '("Int" "Bool"))
(setq fot-types-regexp (regexp-opt fot-types 'words))

(setq fot-operators '(":" "->" "/" "=>" "<"))
(setq fot-operators-regexp (regexp-opt fot-operators))

(setq fot-functions '("mplus" "mtimes" "mdiv" "mminus" "mle"  "bequals" "bnot" "band" "bor"))
(setq fot-functions-regexp (regexp-opt fot-functions 'words))

(setq fot-constants '("true" "false" "mzero" "mone" "mtwo" ))
(setq fot-constants-regexp (regexp-opt fot-constants 'words))

; generate optimized regexp for keywords
;(kill-new (regexp-opt '("sorts" "subsort" "signature" "rewrite") t))

(setq fot-font-lock-keywords
      `(
        (,fot-types-regexp . font-lock-type-face)
        (,fot-constants-regexp . font-lock-constant-face)
        (,fot-operators-regexp . font-lock-builtin-face)
        (,fot-functions-regexp . font-lock-function-name-face)
        (,fot-keywords-regexp . font-lock-keyword-face)
        ;; note: order above matters, because once colored, that part won't change.
        ;; in general, longer words first
        ))


(defvar fot-mode-syntax-table
  (let ((fot-mode-syntax-table (make-syntax-table)))
    (modify-syntax-entry ?/ ". 124b" fot-mode-syntax-table)
    (modify-syntax-entry ?* ". 23" fot-mode-syntax-table)
    (modify-syntax-entry ?\n "> b" fot-mode-syntax-table)
    fot-mode-syntax-table)
  "Syntax table for fot-mode")

(define-derived-mode fot-mode fundamental-mode
  (use-local-map fot-mode-map)
  (set-syntax-table fot-mode-syntax-table)
  (setq font-lock-defaults '((fot-font-lock-keywords)))
  (setq major-mode 'fot-mode)
  (setq mode-name "FOT"))

(provide 'fot-mode)
