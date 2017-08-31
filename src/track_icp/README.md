# LICENSE
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

# Track ICP

## SLAM track ICP with SLAM track
   This module applies slam track registration.

#### Input
1) SLAM-A track files format: x y z t
2) SLAM-B track files format: x y z t

#### Output
1) Registrated SLAM track files format: x y z t

## SLAM track ICP with GPS track
   This module applies weighted slam track registration.

#### Input
1) SLAM track files format: x y z t
2) GPS track files format: x y z t
3) weight coefficient format: w

#### Output
1) Registrated SLAM track files format: x y z t
