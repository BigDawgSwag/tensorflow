# Copyright 2024 The Tensorflow Authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""Tool to verify wheel manylinux compliance."""

import argparse
import manylinux_compliance_utils


def parse_args() -> argparse.Namespace:
  """Arguments parser."""
  parser = argparse.ArgumentParser(
      description="Helper for auditwheel", fromfile_prefix_chars="@"
  )
  parser.add_argument(
      "--wheel-path", required=True, help="Path of the wheel, mandatory"
  )
  parser.add_argument(
      "--compliance-tag", help="ManyLinux compliance tag", required=False
  )
  parser.add_argument(
      "--auditwheel-show-log-path",
      help="Path to file with auditwheel show results, mandatory",
      required=True,
  )
  return parser.parse_args()


if __name__ == "__main__":
  args = parse_args()
  auditwheel_output = manylinux_compliance_utils.get_auditwheel_output(
      args.wheel_path
  )
  manylinux_compliance_utils.verify_manylinux_compliance(
      auditwheel_output,
      args.compliance_tag,
      args.auditwheel_show_log_path,
  )
