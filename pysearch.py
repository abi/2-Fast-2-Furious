import subprocess
import time
import random

nodeamts = [ 10 ** 2
           , 10 ** 3
           , 10 ** 4
           , 10 ** 4 * 5
           , 10 ** 5
           , 10 ** 5 * 2
           , 10 ** 5 * 3
           , 10 ** 5 * 5
           ]

nodeamts = nodeamts[4:]
SINGLE_SCC = 0
MANY_SCC   = 1

results = ""

print "why cdoes god hate me"
for nodeamount in nodeamts:
  step = 1
  trials = 40
  percent_goal = .9
  guess = 1

  step_going_up = True

  normal_going_up = False
  normal_going_down = False
  resolution = 5

  while resolution > 0:
    times = 0

    for x in range(trials):
      name = "temp"

      subprocess.call(["./random", str(nodeamount), str(guess), name,
        str(random.uniform(1, 1000000))])

      if subprocess.call(["./scc", name]) == SINGLE_SCC:
        times += 1

      subprocess.call(["rm", name])

    print "With guess %d, result was %d out of %d times." % (guess, times, trials)
    percent = float(times)/ float(trials)

    print percent

    if step_going_up:
      if percent < percent_goal: 
        guess *= 2
      else:
        step_going_up = False
        normal_going_down = True
        step = guess / 2
        guess -= step
        step /= 2
    else:
      resolution -= 1
      if percent > percent_goal:
        guess -= step
        print "That's too big, trying %d." % guess
        step /= 2
      if percent < percent_goal:
        guess += step
        print "That's too small, trying %d." % guess
        step /= 2

  result = "For %d nodes with %d percent, approx result is %d.\n" % (nodeamount,
      percent_goal, guess)
  results += result

print results




