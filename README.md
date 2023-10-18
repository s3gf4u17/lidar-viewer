### lidar viewer

this prototype allows for quick preview of lidar point clouds in `.las` format. if you're having trouble trying to transform your points between `.laz` to `.las` format i recommend using LAStools available [here](https://github.com/LAStools/LAStools). download the source code, compile it and transform your data using these two useful programs:

```bash
las2las -i <input.laz> -o <output.las>
lasmerge -i <input.laz/input.las> -o <output.laz/output.las>
```

<table width="100%">
  <tr>
  <td width="33.3%" align="center">Warsaw National Stadium</td>
  <td width="33.3%" align="center">Rysy - highest point of Poland</td>
  <td width="33.3%" align="center">The southernmost point of Poland</td>
  </tr>
  <tr>
  <td width="33.3%"><img src="/images/scr1.png?raw=true"/></td>
  <td width="33.3%"><img src="/images/scr2.png?raw=true"/></td>
  <td width="33.3%"><img src="/images/scr3.png?raw=true"/></td>
  </tr>
</table>