//
//  RibbonFunctions.cpp
//
//  Created by David Wicks on 8/30/15.
//
//

#include "RibbonFunctions.h"

using namespace std;
using namespace cinder;

namespace sansumbrella
{

vector<vec3> createRibbon(float width, const std::function<float (float)> &shape_fn, const ci::vec3 &eye_vector, const std::vector<ci::vec3> &spine)
{
  vector<vec3> ribbon;
  ribbon.assign(spine.size() * 2, vec3(0));

  auto half_width = [w = width * 0.3f, &shape_fn] (float v) {
    return w * shape_fn(v);
  };

  vec3 a, b;
  const auto last_index = spine.size() - 1.0f;
  // first vertex
  a = spine.at( 0 );
  b = spine.at( 1 );
  vec3 edge = normalize(b - a);
  vec3 tangent = cross(edge, eye_vector);
  vec3 north = normalize(tangent) * half_width( 0 );
  vec3 south = -north;
  ribbon.at( 0 ) = a + north;
  ribbon.at( 0 + 1) = a + south;
  // remaining vertices
  for( int i = 1; i < spine.size(); ++i )
  {
    a = spine.at(i - 1);
    b = spine.at(i);
    edge = normalize(b - a);
    tangent = cross(edge, eye_vector);
    north = normalize(tangent) * half_width( i / last_index );
    south = -north;
    ribbon.at( i * 2 ) = b + north;
    ribbon.at( i * 2 + 1 ) = b + south;
  }

  return ribbon;
}

} // namespace sansumbrella
