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

# GPS Process
## Gaussian transform or UTM transform

### Gaussian transform

Gaussian transform is a method that transform WGS84 GPS coordinates to local coordinates. The x-axis of the local coordinate system is the central meridian and the north is the positive direction; the y-axis of the local coordinate system is the equatorial and the east is the positive direction. The Gaussian transform is an isometric transformation. In order to improve transform accuracy, data must be zoned which named band, each size of band is 3 degrees or 6 degrees, and the middle of the meridian is central meridian.

### UTM transform
UTM transform is similar with Gaussian transform, but UTM transform cannot transform from 84 degrees to 90 degrees in North or from 80 degrees to 90 degrees in South. The length of the central meridian becomes 0.9996 times.

### Positive transform
From WGS-84 to ENU

#### Input
1) SLAM track files format: x y z t
2) original GPS files format: RMC
3) Method format: Gaussian or UTM-(UTM Recommended)
4) type format:3 or 6-(3 Recommended) 

#### Output
1) SLAM track files format: x y z t
2) GPS track files format: x y z t

### Reverse transform
From ENU to WGS-84

#### Input
1) processed gps track files format:x y z t (w)
2) Method format:Gaussion or UTM-(UTM Recommended)
3) type format:3 or 6-(3 Recommended)

#### Output
1) google earth files format-kml
