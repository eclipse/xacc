/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 *License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
#ifndef XACC_UTILS_SINGLETON_HPP_
#define XACC_UTILS_SINGLETON_HPP_

namespace xacc {

/**
 * Singleton provides a templated implementation of
 * the Singleton Design Pattern. This class takes a template parameter
 * and provides behviour around that template that models
 * a singleton - ie there is only one instance available during runtime.
 */
template <class T> class Singleton {
public:
  /**
   * Return the single instance of T
   * @return instance The singleton instance
   */
  static T *instance() {
    if (!instance_) {
      instance_ = new T();
    }
    return instance_;
  }

  /**
   * Destroy the single instance of T
   */
  static void destroy() {
    delete instance_;
    instance_ = nullptr;
  }

protected:
  /**
   * Reference to the single T instance
   */
  static T *instance_;

  /**
   * constructor
   */
  inline explicit Singleton() {}

  /**
   * destructor
   */
  virtual ~Singleton() {}
};

template <class T> T *Singleton<T>::instance_ = nullptr;

} // namespace xacc

#endif
