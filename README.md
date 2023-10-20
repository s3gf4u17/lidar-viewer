### lidar viewer

this prototype allows for quick preview of lidar point clouds in `.las` format. if you're having trouble trying to transform your points between `.laz` to `.las` format i recommend using LAStools available [here](https://github.com/LAStools/LAStools). download the source code, compile it and transform your data using these two useful programs:

```bash
las2las -i <input.laz> -o <output.las>
lasmerge -i <input.laz/input.las> -o <output.laz/output.las>
```

## Examples

<table width="100%">
  <tr>
  <td width="100.0%" align="center">Fragment of the Tatra National Park (57 049 110 points/1.9 GB)</td>
  </tr>
  <tr>
  <td width="100.0%"><img src="/images/scr1.png?raw=true"/></td>
  </tr>
  <tr>
  <td width="100.0%" align="center">Polsat Plus Arena Gdańsk (23 700 686 points/769 MB)</td>
  </tr>
  <tr>
  <td width="100.0%"><img src="/images/scr3.png?raw=true"/></td>
  </tr>
  <tr>
  <td width="100.0%" align="center">Jaworzno Power Station (60 298 174 points/2.0 GB)</td>
  </tr>
  <tr>
  <td width="100.0%"><img src="/images/scr2.png?raw=true"/></td>
  </tr>
</table>