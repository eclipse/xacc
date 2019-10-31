/**
 * @file rmsprop.hpp
 * @author Ryan Curtin
 * @author Marcus Edel
 * @author Vivek Pal
 *
 * RMSProp optimizer. RMSProp is an optimizer that utilizes the magnitude of
 * recent gradients to normalize the gradients.
 *
 * ensmallen is free software; you may redistribute it and/or modify it under
 * the terms of the 3-clause BSD license.  You should have received a copy of
 * the 3-clause BSD license along with ensmallen.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef ENSMALLEN_RMSPROP_RMSPROP_HPP
#define ENSMALLEN_RMSPROP_RMSPROP_HPP

#include <ensmallen_bits/sgd/sgd.hpp>
#include "rmsprop_update.hpp"

namespace ens {

/**
 * RMSProp is an optimizer that utilizes the magnitude of recent gradients to
 * normalize the gradients. In its basic form, given a step rate \f$ \gamma \f$
 * and a decay term \f$ \alpha \f$ we perform the following updates:
 *
 * \f{eqnarray*}{
 * r_t &=& (1 - \gamma) f'(\Delta_t)^2 + \gamma r_{t - 1} \\
 * v_{t + 1} &=& \frac{\alpha}{\sqrt{r_t}}f'(\Delta_t) \\
 * \Delta_{t + 1} &=& \Delta_t - v_{t + 1}
 * \f}
 *
 * For more information, see the following.
 *
 * @code
 * @misc{tieleman2012,
 *   title = {Lecture 6.5 - rmsprop, COURSERA: Neural Networks for Machine
 *            Learning},
 *   year  = {2012}
 * }
 * @endcode
 *
 * RMSProp can optimize differentiable separable functions.  For more details,
 * see the documentation on function types included with this distribution or on
 * the ensmallen website.
 */
class RMSProp
{
 public:
  /**
   * Construct the RMSProp optimizer with the given function and parameters. The
   * defaults here are not necessarily good for the given problem, so it is
   * suggested that the values used be tailored to the task at hand.  The
   * maximum number of iterations refers to the maximum number of points that
   * are processed (i.e., one iteration equals one point; one iteration does not
   * equal one pass over the dataset).
   *
   * @param stepSize Step size for each iteration.
   * @param batchSize Number of points to process in each step.
   * @param alpha Smoothing constant, similar to that used in AdaDelta and
   *        momentum methods.
   * @param epsilon Value used to initialise the mean squared gradient parameter.
   * @param maxIterations Maximum number of iterations allowed (0 means no
   *        limit).
   * @param tolerance Maximum absolute tolerance to terminate algorithm.
   * @param shuffle If true, the function order is shuffled; otherwise, each
   *        function is visited in linear order.
   * @param resetPolicy If true, parameters are reset before every Optimize
   *        call; otherwise, their values are retained.
   * @param exactObjective Calculate the exact objective (Default: estimate the
   *        final objective obtained on the last pass over the data).
   */
  RMSProp(const double stepSize = 0.01,
          const size_t batchSize = 32,
          const double alpha = 0.99,
          const double epsilon = 1e-8,
          const size_t maxIterations = 100000,
          const double tolerance = 1e-5,
          const bool shuffle = true,
          const bool resetPolicy = true,
          const bool exactObjective = false) :
      optimizer(stepSize,
                batchSize,
                maxIterations,
                tolerance,
                shuffle,
                RMSPropUpdate(epsilon, alpha),
                NoDecay(),
                resetPolicy,
                exactObjective)
  { /* Nothing to do. */ }

  /**
   * Optimize the given function using RMSProp. The given starting point will be
   * modified to store the finishing point of the algorithm, and the final
   * objective value is returned.
   *
   * @tparam DecomposableFunctionType Type of the function to be optimized.
   * @tparam MatType Type of matrix to optimize with.
   * @tparam GradType Type of matrix to use to represent function gradients.
   * @tparam CallbackTypes Types of callback functions.
   * @param function Function to optimize.
   * @param iterate Starting point (will be modified).
   * @param callbacks Callback functions.
   * @return Objective value of the final point.
   */
  template<typename DecomposableFunctionType,
           typename MatType,
           typename GradType,
           typename... CallbackTypes>
  typename std::enable_if<IsArmaType<GradType>::value,
      typename MatType::elem_type>::type
  Optimize(DecomposableFunctionType& function,
           MatType& iterate,
           CallbackTypes&&... callbacks)
  {
    return optimizer.Optimize<DecomposableFunctionType, MatType, GradType,
        CallbackTypes...>(function, iterate, callbacks...);
  }

  //! Forward the MatType as GradType.
  template<typename DecomposableFunctionType,
           typename MatType,
           typename... CallbackTypes>
  typename MatType::elem_type Optimize(DecomposableFunctionType& function,
                                       MatType& iterate,
                                       CallbackTypes&&... callbacks)
  {
    return Optimize<DecomposableFunctionType, MatType, MatType,
        CallbackTypes...>(function, iterate,
        std::forward<CallbackTypes>(callbacks)...);
  }

  //! Get the step size.
  double StepSize() const { return optimizer.StepSize(); }
  //! Modify the step size.
  double& StepSize() { return optimizer.StepSize(); }

  //! Get the batch size.
  size_t BatchSize() const { return optimizer.BatchSize(); }
  //! Modify the batch size.
  size_t& BatchSize() { return optimizer.BatchSize(); }

  //! Get the smoothing parameter.
  double Alpha() const { return optimizer.UpdatePolicy().Alpha(); }
  //! Modify the smoothing parameter.
  double& Alpha() { return optimizer.UpdatePolicy().Alpha(); }

  //! Get the value used to initialise the mean squared gradient parameter.
  double Epsilon() const { return optimizer.UpdatePolicy().Epsilon(); }
  //! Modify the value used to initialise the mean squared gradient parameter.
  double& Epsilon() { return optimizer.UpdatePolicy().Epsilon(); }

  //! Get the maximum number of iterations (0 indicates no limit).
  size_t MaxIterations() const { return optimizer.MaxIterations(); }
  //! Modify the maximum number of iterations (0 indicates no limit).
  size_t& MaxIterations() { return optimizer.MaxIterations(); }

  //! Get the tolerance for termination.
  double Tolerance() const { return optimizer.Tolerance(); }
  //! Modify the tolerance for termination.
  double& Tolerance() { return optimizer.Tolerance(); }

  //! Get whether or not the individual functions are shuffled.
  bool Shuffle() const { return optimizer.Shuffle(); }
  //! Modify whether or not the individual functions are shuffled.
  bool& Shuffle() { return optimizer.Shuffle(); }

  //! Get whether or not the actual objective is calculated.
  bool ExactObjective() const { return optimizer.ExactObjective(); }
  //! Modify whether or not the actual objective is calculated.
  bool& ExactObjective() { return optimizer.ExactObjective(); }

  //! Get whether or not the update policy parameters
  //! are reset before Optimize call.
  bool ResetPolicy() const { return optimizer.ResetPolicy(); }
  //! Modify whether or not the update policy parameters
  //! are reset before Optimize call.
  bool& ResetPolicy() { return optimizer.ResetPolicy(); }

 private:
  //! The Stochastic Gradient Descent object with RMSPropUpdate policy.
  SGD<RMSPropUpdate> optimizer;
};

} // namespace ens

#endif
