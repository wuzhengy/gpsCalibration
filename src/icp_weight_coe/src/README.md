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

# ICP weight coefficient
   The module has two functions.

## Speed weight coefficient calculation
   This part of weight is proportional to speed.

### Input 
    lidar slam track: x y z t

### Output 
    speed weight: w

## GPS weight coefficient calculation
   The distance between processed lidar slam track and GPS track gives this part of weight.

### Input 
    lidar slam track: x y z t
    original gps track: x y z t

### Output 
    speed weight: w
