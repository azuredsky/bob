/**
 * @file visioner/programs/readmit+cmu.cc
 * @date Fri 27 Jul 13:58:57 2012 CEST
 * @author Andre Anjos <andre.anjos@idiap.ch>
 *
 * @brief This file was part of Visioner and originally authored by "Cosmin
 * Atanasoaei <cosmin.atanasoaei@idiap.ch>". It was only modified to conform to
 * Bob coding standards and structure.
 *
 * Copyright (C) 2011-2012 Idiap Research Institute, Martigny, Switzerland
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

#include "core/logging.h"

#include "visioner/vision/object.h"

// Parse a MIT+CMU ground truth file
bool parse(const bob::visioner::string_t& file)
{
  // Load file content
  bob::visioner::string_t text;
  if (bob::visioner::load_file(file, text) == false)
  {
    bob::core::error << "Failed to load <" << file << ">!" << std::endl;
    return false;
  }

  static const bob::visioner::index_t n_points = 6;
  static const bob::visioner::string_t points[n_points] = 
  {
    "leye", "reye", "nose", "lmc", "mc", "rmc"      
  };

  const bob::visioner::strings_t lines = bob::visioner::split(text, "\n");
  for (bob::visioner::index_t i = 0; i < lines.size(); i ++)
  {
    const bob::visioner::strings_t tokens = bob::visioner::split(lines[i], "\t {}");
    if (tokens.size() != 2 * n_points + 1)
    {
      continue;
    }

    const bob::visioner::string_t ifile = tokens[0];
    const bob::visioner::string_t gfile = bob::visioner::basename(ifile) + ".gt";

    bob::visioner::Object object("face", "unknown", "unknown");    

    for (bob::visioner::index_t j = 0; j < n_points; j ++)
    {
      const bob::visioner::string_t x = tokens[2 * j + 1];
      const bob::visioner::string_t y = tokens[2 * j + 2];

      object.add(bob::visioner::Keypoint(
            points[j], 
            boost::lexical_cast<float>(x),
            boost::lexical_cast<float>(y)));
    }

    bob::visioner::objects_t objects;
    bob::visioner::Object::load(gfile, objects);

    objects.push_back(object);
    bob::visioner::Object::save(gfile, objects); 
  }

  // OK
  return true;
}

int main(int argc, char *argv[]) {	

  const bob::visioner::string_t input = "annotations";

  parse(input);

  // OK
  bob::core::info << "Program finished successfuly" << std::endl;
  return EXIT_SUCCESS;
}