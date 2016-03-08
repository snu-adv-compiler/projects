;;=======================================
;;     miscellaneous options
;;=======================================

(setq scroll-step 1) ;; scroll just 1 line when hitting the bottom

(show-paren-mode t) ;; show matching parenthesis

(transient-mark-mode t) ;; highlight marked region

(column-number-mode 1) ;; show column number

;; show a menu only when running within X (save real estate when in console)
(menu-bar-mode (if window-system 1 -1))

(display-time-mode 0) ;; Korean text in time display causes error sometimes

(ido-mode t) ;; buffer-navigation, find-file extension

;; open in makefile-mode if filename "contains" Makefile
(add-to-list 'auto-mode-alist '("Makefile" . makefile-mode))

(add-hook 'c-mode-common-hook
	  (lambda ()
	    (modify-syntax-entry ?_ "w") ;; regard underscore as part of the word
	    (setq indent-tabs-mode nil) ;; don't include tabs in indent
	    (setq show-trailing-whitespace t)))
