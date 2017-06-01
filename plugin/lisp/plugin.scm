(define-module (plugin)
  #:export (run_ls))

(define (run_ls datafd logfd)
  (write "In Lisp\n")
  (run_ls datafd logfd))
