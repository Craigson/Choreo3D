//
//  RibbonFunctions.h
//
//  Created by David Wicks on 8/30/15.
//
//

#pragma once

namespace sansumbrella
{

  std::vector<ci::vec3> createRibbon(float width, const std::function<float (float)> &shape_fn, const ci::vec3 &eye_vector, const std::vector<ci::vec3> &spine);

} // namespace sansumbrella
