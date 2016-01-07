#pragma once

#include "cinder/Json.h"
#include "CCL_MocapJoint.h"
#include "cinder/Vector.h"
#include "Jzon.h"

namespace ccl
{

std::vector<CCL_MocapJoint> loadMotionCaptureFromJson(const ci::fs::path &file);
std::vector<CCL_MocapJoint> loadMotionCaptureFromSite(const ci::Url &url, int skip);

void addUUIDtoJoint(const std::string &grp, const std::string &title, const std::string &uuid, int skip, std::vector<CCL_MocapJoint>& output_joints);

const std::string URL_STREAM_JSON = "http://api.piecemeta.com/channels/c51be7cc-a985-4c4a-a53b-fa034d33fff8/streams.json";
const std::string URL_STREAM_REQ = "http://api.piecemeta.com/streams/";

} // namespace ccl
