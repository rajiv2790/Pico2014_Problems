def grade(arg, key):
        if "i_will_be_careful_with_sigreturn" in key:
            return True, "Correct"
        else:
            return False, "Incorrect"
