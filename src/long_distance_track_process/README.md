#LICENSE
Copyright @2017 The iMorpheusAI Authors
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

# Long distance track process
This package completes two functions:
1) GPS track is transformed from WGS-84 to ENU coordinates.
2) Registration of SLAM track with original gps track.

## Input
1) SLAM track files format: x y z t
2) GPS track files format: x y z t
3) weight coefficient file format: w
4) ICP type: 1

## Output
1) ENU Track temp files format: x y z t
2) Registrated SLAM Track temp files format: x y z t
