/*
 * superviseddescent: A C++11 implementation of the supervised descent
 *                    optimisation method
 * File: superviseddescent/matserialisation.hpp
 *
 * Copyright 2014 Patrik Huber
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#pragma once

#ifndef MATSERIALISATION_HPP_
#define MATSERIALISATION_HPP_

#include "opencv2/core/core.hpp"

#ifdef WIN32
	#define BOOST_ALL_DYN_LINK	// Link against the dynamic boost lib. Seems to be necessary because we use /MD, i.e. link to the dynamic CRT.
	#define BOOST_ALL_NO_LIB	// Don't use the automatic library linking by boost with VS2010 (#pragma ...). Instead, we specify everything in cmake.
#endif
#include "boost/serialization/serialization.hpp"
#include "boost/serialization/array.hpp"

/**
 * Serialisation for the OpenCV cv::Mat class. Supports text and binary serialisation.
 *
 * Based on answer from: http://stackoverflow.com/questions/4170745/serializing-opencv-mat-vec3f 
 * Different method and tests: http://cheind.wordpress.com/2011/12/06/serialization-of-cvmat-objects-using-boost/
 *
 * Todos:
 *  - Add the unit tests from above blog.
 *  - Add XML support (need to add make_nvp stuff?)
 */
namespace boost {
	namespace serialization {

		/**
		 * Serialize a cv::Mat using boost::serialization.
		 *
		 * Supports all types of matrices as well as non-contiguous ones.
		 
		 * @param[in] ar The archive to serialise to (or to serialise from).
		 * @param[in] mat The matrix to serialise (or deserialise).
		 * @param[in] unused An optional version argument.
		 */
		template<class Archive>
		void serialize(Archive &ar, cv::Mat& mat, const unsigned int)
		{
			int rows, cols, type;
			bool continuous;

			if (Archive::is_saving::value) {
				rows = mat.rows; cols = mat.cols; type = mat.type();
				continuous = mat.isContinuous();
			}

			ar & rows & cols & type & continuous;

			if (Archive::is_loading::value)
				mat.create(rows, cols, type);

			if (continuous) {
				const unsigned int data_size = rows * cols * static_cast<int>(mat.elemSize());
				ar & boost::serialization::make_array(mat.ptr(), data_size);
			}
			else {
				const unsigned int row_size = cols * static_cast<int>(mat.elemSize());
				for (int i = 0; i < rows; i++) {
					ar & boost::serialization::make_array(mat.ptr(i), row_size);
				}
			}
		};

	} /* namespace serialization */
} /* namespace boost */

#endif /* MATSERIALISATION_HPP_ */
