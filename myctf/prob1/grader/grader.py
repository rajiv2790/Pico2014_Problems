def grade(arg, key):
        if "shadow_stack_makes_life_harder" in key:
            return True, "Correct"
        else:
            return False, "Incorrect"
