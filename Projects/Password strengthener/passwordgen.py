import random

lowercase = "abcdefghijklmnopqrstuvwxyz"
uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
schars = "[]()$#*;@_-,."
nums = "1234567890"

all = lowercase + uppercase + schars + nums
length = int(input("How many characters do you want your password to be? "))
password = "".join(random.sample(all, length))
print(password)