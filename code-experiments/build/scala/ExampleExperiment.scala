import scala.util.control.Breaks._
import scala.util.Random

object ExampleExperiment {

  /**
    * The maximal budget for evaluations done by an optimization algorithm equals
    * dimension * BUDGET_MULTIPLIER.
    * Increase the budget multiplier value gradually to see how it affects the runtime.
    */
  val BudgetMultiplier = 4

  /**
    * The maximal number of independent restarts allowed for an algorithm that restarts itself.
    */
  val IndependentRestarts = 10000

  /**
    * The random seed. Change if needed.
    */
  val RandomSeed = 0xdeadbeef

  /**
    * A simple random search algorithm that can be used for single- as well as multi-objective
    * optimization.
    */

  type Vector = Array[Double]

  trait Function {
    def evaluate(x: Vector): Vector

    def evaluateConstraint(x: Vector): Vector
  }


  def myRandomSearch(f: Function,
                     dimension: Int,
                     numberOfObjectives: Int,
                     numberOfConstraints: Int,
                     lowerBounds: Vector,
                     upperBounds: Vector,
                     maxBudget: Long,
                     randomGenerator: Random): Unit = {

    var i = 0L
    while (i < maxBudget) {
      i += 1

      /* Construct x as a random point between the lower and upper bounds */
      val x = (upperBounds, lowerBounds).zipped.map { case (ub, lb) => lb + randomGenerator.nextDouble() * (ub - lb) }

      /* Call the evaluate function to evaluate x on the current problem (this is where all the COCO logging
       * is performed) */
      if (numberOfConstraints > 0) {
        val z = f.evaluateConstraint(x)
      }
      val y = f.evaluate(x)
    }
  }

  def exampleExperiment(suiteName: String, observerName: String, randomGenerator: Random): Unit = {

    /* Set some options for the observer. See documentation for other options. */
    val observerOptions =
      "result_folder: RS_on_" + suiteName + " " +
        "algorithm_name: RS " +
        "algorithm_info: \"A simple random search algorithm\""

    /* Initialize the suite and observer.
           * For more details on how to change the default options, see
           * http://numbbo.github.io/coco-doc/C/#suite-parameters and
           * http://numbbo.github.io/coco-doc/C/#observer-parameters. */
    val suite = new Suite(suiteName, "", "")
    val observer = new Observer(observerName, observerOptions)
    val benchmark = new Benchmark(suite, observer)

    /* Initialize timing */
    val timing = new Timing()

    var problem: Problem = null
    /* Iterate over all problems in the suite */
    while ( {
      problem = benchmark.getNextProblem
      problem != null
    }) {

      val dimension = problem.getDimension

      /* Run the algorithm at least once */
      breakable {
        for (_ <- 1 to IndependentRestarts) {

          val evaluationsDone = problem.getEvaluations + problem.getEvaluationsConstraints
          val evaluationsRemaining = (dimension * BudgetMultiplier) - evaluationsDone

          /* Break the loop if the target was hit or there are no more remaining evaluations */
          if (problem.isFinalTargetHit || (evaluationsRemaining <= 0))
            break

          val evaluateFunction = new Function {
            override def evaluateConstraint(x: Vector): Vector = {
              problem.evaluateConstraint(x)
            }

            override def evaluate(x: Vector): Vector = {
              problem.evaluateFunction(x)
            }
          }

          /* Call the optimization algorithm for the remaining number of evaluations */
          myRandomSearch(evaluateFunction,
            dimension,
            problem.getNumberOfObjectives,
            problem.getNumberOfConstraints,
            problem.getSmallestValuesOfInterest,
            problem.getLargestValuesOfInterest,
            evaluationsRemaining,
            randomGenerator)

          /* Break the loop if the algorithm performed no evaluations or an unexpected thing happened */
          if (problem.getEvaluations <= evaluationsDone) {
            if (problem.getEvaluations == evaluationsDone) {
              println("WARNING: Budget has not been exhausted (" + evaluationsDone + "/"
                + dimension * BudgetMultiplier + " evaluations done)!\n")
            } else {
              println("ERROR: Something unexpected happened - function evaluations were decreased!")
            }
            break
          }
        }
      }

      /* Keep track of time */
      timing.timeProblem(problem)
    }

    /* Output the timing data */
    timing.output()

    benchmark.finalizeBenchmark()
  }

  /**
    * The problem to be optimized (needed in order to simplify the interface between the optimization
    * algorithm and the COCO platform).
    */
  //var Problem PROBLEM

  def main(args: Array[String]): Unit = {
    val randomGenerator = new Random(RandomSeed)

    /* Change the log level to "warning" to get less output */
    CocoJNI.cocoSetLogLevel("info")

    println("Running the example experiment... (might take time, be patient)")

    /* Start the actual experiments on a test suite and use a matching logger, for
         * example one of the following:
         *
         *   bbob                 24 unconstrained noiseless single-objective functions
         *   bbob-biobj           55 unconstrained noiseless bi-objective functions
         *   bbob-biobj-ext       92 unconstrained noiseless bi-objective functions
         *   bbob-largescale      24 unconstrained noiseless single-objective functions in large dimension
         *   bbob-constrained     48 constrained noiseless single-objective functions
         *
         * Adapt to your need. Note that the experiment is run according
         * to the settings, defined in exampleExperiment(...) below.
         */
    exampleExperiment("bbob", "bbob", randomGenerator)

    println("Done!")

  }
}