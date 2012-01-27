/**
 * @author Andre Anjos <andre.anjos@idiap.ch>
 * @date Fri  2 Dec 17:52:32 2011 
 *
 * @brief C++ bindings to libsvm
 *
 * Copyright (C) 2011 Idiap Reasearch Institute, Martigny, Switzerland
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BOB_MACHINE_SVM_H 
#define BOB_MACHINE_SVM_H

#include <svm.h>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include <blitz/array.h>
#include <fstream>

namespace bob { namespace machine {

  /**
   * Loads a given libsvm data file. The data file format, as defined on the
   * library README is like this:
   *
   * <label> <index1>:<value1> <index2>:<value2> ...
   * .
   * .
   * .
   *
   * The labels are integer values, so are the indexes, starting from "1" (and
   * not from zero as a C-programmer would expect. The values are floating
   * point.
   *
   * Zero values are suppresed - this is a sparse format.
   */
  class SVMFile {

    public: //api

      /**
       * Constructor, initializes the file readout.
       */
      SVMFile (const std::string& filename, size_t shape);

      /**
       * Destructor virtualization
       */
      virtual ~SVMFile();

      /**
       * Returns the size of each entry in the file, in number of floats
       */
      inline size_t shape() const { return m_shape; }

      /**
       * Resets the file, going back to the beginning.
       */
      void reset();

      /**
       * Reads the next entry. Values are organized according to the indexed
       * labels at the file. Returns 'false' if the file is over or something
       * goes wrong.
       */
      bool read(int& label, blitz::Array<double,1>& values);

      /**
       * Reads the next entry on the file, but without checking. Returns
       * 'false' if the file is over or something goes wrong reading the file.
       */
      bool read_(int& label, blitz::Array<double,1>& values);

      /**
       * Returns the name of the file being read.
       */
      inline const std::string& filename() const { return m_filename; }

      /**
       * Tests if the file is still good to go.
       */
      inline bool good() const { return m_file.good(); }
      inline bool eof() const { return m_file.eof(); }
      inline bool fail() const { return m_file.fail(); }

    private: //representation

      std::string m_filename; ///< The path to the file being read
      std::ifstream m_file; ///< The file I'm reading.
      size_t m_shape; ///< Number of floats in samples

  };

  /**
   * Interface to svm_model, from libsvm. Incorporates prediction.
   */
  class SupportVector {

    public: //api

      enum svm_t { 
        C_SVC, 
        NU_SVC, 
        ONE_CLASS, 
        EPSILON_SVR, 
        NU_SVR 
      }; /* svm_type */

      enum kernel_t { 
        LINEAR, 
        POLY, 
        RBF, 
        SIGMOID, 
        PRECOMPUTED 
      }; /* kernel_type */

      /**
       * Builds a new SVM model from a libsvm model file. Throws if a problem
       * is found.
       */
      SupportVector(const char* model_file);

      /**
       * Virtual d'tor
       */
      virtual ~SupportVector();

      /**
       * Tells the input size this machine expects
       */
      size_t inputSize() const;

      /**
       * The number of outputs depends on the number of classes the machine has
       * to deal with. If the problem is a regression problem, the number of
       * outputs is fixed to 1. The same happens in a binary classification
       * problem. Otherwise, the output size is the same as the number of
       * classes being discriminated.
       */
      size_t outputSize() const;

      /**
       * Tells the number of classes the problem has.
       */
      size_t numberOfClasses() const;

      /**
       * Returns the class label (as stored inside the svm_model object) for a
       * given class 'i'.
       */
      int classLabel(size_t i) const;

      /**
       * SVM type
       */
      svm_t machineType() const;

      /**
       * Kernel type
       */
      kernel_t kernelType() const;

      /**
       * Polinomial degree, if kernel is POLY
       */
      int polynomialDegree() const;

      /**
       * Gamma factor, for POLY, RBF or SIGMOID kernels
       */
      double gamma() const;

      /**
       * Coefficient 0 for POLY and SIGMOID kernels
       */
      double coefficient0() const;

      /**
       * Tells if this model supports probability output.
       */
      bool supportsProbability() const;

      /**
       * Predict, output classes only. Note that the number of labels in the
       * output "labels" array should be the same as the number of input.
       */
      int predictClass(const blitz::Array<double,1>& input) const;

      /**
       * Predict, output classes only. Note that the number of labels in the
       * output "labels" array should be the same as the number of input.
       *
       * This does the same as predictClass(), but does not check the input.
       */
      int predictClass_(const blitz::Array<double,1>& input) const;

      /**
       * Predicts class and scores output for each class on this SVM,
       *
       * Note: The output array must be lying on contiguous memory. This is
       * also checked.
       */
      int predictClassAndScores
        (const blitz::Array<double,1>& input,
         blitz::Array<double,1>& scores) const;

      /**
       * Predicts output class and scores. Same as above, but does not check
       */
      int predictClassAndScores_
        (const blitz::Array<double,1>& input,
         blitz::Array<double,1>& scores) const;

      /**
       * Predict, output class and probabilities for each class on this SVM,
       * but only if the model supports it. Otherwise, throws a run-time
       * exception.
       *
       * Note: The output array must be lying on contiguous memory. This is
       * also checked.
       */
      int predictClassAndProbabilities
        (const blitz::Array<double,1>& input, 
         blitz::Array<double,1>& probabilities) const;

      /**
       * Predict, output class and probability, but only if the model supports
       * it. Same as above, but does not check
       */
      int predictClassAndProbabilities_
        (const blitz::Array<double,1>& input,
         blitz::Array<double,1>& probabilities) const;

      /**
       * Saves the current model state to a libsvm file.
       */
      void save(const char* filename) const;

    private: //representation

      boost::shared_ptr<svm_model> m_model; ///< libsvm model pointer
      size_t m_input_size; ///< vector size expected as input for the SVM's
      mutable boost::shared_array<svm_node> m_input_cache; ///< cache

  };

}}

#endif /* BOB_MACHINE_SVM_H */