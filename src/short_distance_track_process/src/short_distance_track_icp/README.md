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


#short-distance-track-icp
transform local coordinate from slam track into enu coordinate using ICP algorithm. there are two methods.when gps track's weight is high,then do icp with weight and get enu track;when gps track's weight is low,then do icp using neighbor slam enu track that has done icp.every slam track has overlap with its neighbor slam track.

#input
gps track with weight
slam track
#output
enu track which has down icp
