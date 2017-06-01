(define-module (plugin)
  #:export (run_ls))

(define (run_ls datafd logfd read_ls write_ls send_ls)
  (read-loop datafd logfd read_ls write_ls send_ls))

(define (read-loop datafd logfd recv write send)
  (let ((line (recv datafd)))
    (handle line logfd write send)
    (read-loop datafd logfd recv write send)))

(define (handle line logfd write send)
  (write logfd "+40 In Lisp")
  (write logfd (string-append "+40 LS-" line)))
