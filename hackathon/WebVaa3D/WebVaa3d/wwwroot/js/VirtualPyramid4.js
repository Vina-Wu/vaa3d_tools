﻿const axios = window.axios;
var canvas = document.createElement('canvas');
var context = canvas.getContext('webgl2');
renderer = new THREE.WebGLRenderer({ canvas: canvas, context: context });
//camera = new THREE.PerspectiveCamera();
//scene = new THREE.Scene();
mouse = new THREE.Vector2();
clock = new THREE.Clock();
bbox = new THREE.BoxGeometry(537, 853, 153);
level = 0;

scene = [];
tiffwidth = 0;
tiffheight = 0;

rgba = new Uint8Array();
for (var i = 0; i <= 5; i++) {

    scene[i] = new THREE.Scene;
}

//console.log(scene);
camera = [];
for (var i = 0; i <= 5; i++) {

    camera[i] = new THREE.PerspectiveCamera();
}

controls = new THREE.OrbitControls(camera[level]);//创建控件对象




var spheres = [];
var spheresIndex = 0;



function init() {
    width = document.getElementById('view').clientWidth;
    height = document.getElementById('view').clientHeight;
    renderer.setSize(width, height);

    for (var i = 0; i <= 5; i++) {

         camera[i] = new THREE.PerspectiveCamera(45, width / height, 1, 10000);
        //var h = 512; // frustum height
        //var aspect = width / height;
        //camera[i] = new THREE.OrthographicCamera(- h * aspect / 2, h * aspect / 2, h / 2, - h / 2, 1, 10000);
    }

    document.getElementById("view").appendChild(renderer.domElement);

    controls = new THREE.OrbitControls(camera[0], document.getElementById('view'));//创建控件对象
    controls.addEventListener('change', render);//监听鼠标、键盘事件
    controls.target.set(269, 427, 77);

    camera[0].position.x = 431;
    camera[0].position.y = 494;
    camera[0].position.z = 1245;

    //renderer.setClearColor(0x000000, 1.0);
    renderer.setClearColor(0x000000, 1.0);
    //renderer.setPixelRatio(window.devicePixelRatio);
    document.getElementById("view").appendChild(renderer.domElement);


    //var material = new THREE.MeshBasicMaterial({ color: 0x000000 });
    //material.transparent = true;
    //material.opacity = 0;
    //var cube = new THREE.Mesh(bbox, material);
    //cube.position.set(427, 269, 77);
    //console.log(bbox);
    //scene.add(cube);


    //camera[0].lookAt(269, 427, 77);
    // camera[0].up.set(1, 0, 0);

    var axesHelper = new THREE.AxesHelper(1000);
    scene[0].add(axesHelper);



    //console.log(cube);
    render();

}

function render() {
    //requestAnimationFrame(render);
    renderer.render(scene[level], camera[level]);

}
//console.log(scene.children);
//scene.children.material.blending = THREE["CustomBlending"];
//scene.children.material.blendEquation = THREE.MaxEquation;
render();

//function renderscene2() {
//    //requestAnimationFrame(render);
//    renderer.render(scene2, camera2);

//}


async function getPictures() {
    init();
    var id = 0;
    const res = await axios.get(`api/tiff/${+id}`);
    var files = res.data;
    for (index in files) {
        const file = files[index];
        const response = await axios.get(`api/tiff/${+id}/${file}`, { responseType: "arraybuffer" });


        var fileName = $("#file").val();
        console.log(file);
       
        var regex = /\d+/g;
        var filestr = file.match(regex);
        //console.log(Math.round(filestr[1] / 114560), int(filestr[0] / 136960));
        var tiff = new Tiff({ buffer: response.data });
        var count = tiff.countDirectory();
        tiff.setDirectory(0);
        //var rgba = new Uint8Array(tiff.readRGBAImage());
        //rgba = tiff.readRGBAImage();
        //console.log(rgba);

        if (index == 0) {
            rgba = new Uint8Array(tiff.width()*tiff.height()*count*4*12);
           // console.log(rgba);
            console.log(tiff.width(), tiff.height(), count);
        }

        //if (index == 0) {
        //    var rgba = new Uint8Array(tiff.readRGBAImage());
            
        //    console.log(rgba);
        //}
      

        var j;
        for (j = 0; j < count; j++) {
           
            tiff.setDirectory(j);
            var buffer = new Uint8Array(tiff.readRGBAImage());
            var k; var contrast = 12; var thresh = 0;
            for (k = 0; k < buffer.byteLength; k = k + 4) {
                if (buffer[k] < thresh) { buffer[k + 3] = 0; }
                buffer[k] *= contrast;
                buffer[k + 1] *= contrast;
                buffer[k + 2] *= contrast;
                if (buffer[k] == 0 && buffer[k + 1] == 0 && buffer[k + 2] == 0)
                    buffer[k + 3] = 0;

            }
            //console.log(buffer);
            //console.log(buffer.slice(0,  tiff.width() * 4));
            for (x = 0; x < tiff.height(); x++) {
                rgba.set(buffer.slice(x * tiff.width() * 4, (x + 1) * tiff.width() * 4), j * tiff.width() * tiff.height() * 4 * 12 + (Math.round(filestr[0] / 136960 )* tiff.height() + x) * 3 * tiff.width() * 4 + Math.round(filestr[1] / 114560) * tiff.width() * 4);


            }
            //console.log(rgba);
           


            //console.log(rgba, buffer);
           // rgba = mergeTypedArraysUnsafe(rgba, buffer);
            buffer = null;
        }

        //var k; var contrast = 12; var thresh = 0;
        //for (k = 0; k < rgba.byteLength; k = k + 4) {
        //    if (rgba[k] < thresh) { rgba[k + 3] = 0; }
        //    rgba[k] *= contrast;
        //    rgba[k + 1] *= contrast;
        //    rgba[k + 2] *= contrast;
        //    if (rgba[k] == 0 && rgba[k + 1] == 0 && rgba[k + 2] == 0)
        //        rgba[k + 3] = 0;

        //}
       // console.log(rgba);
        tiffwidth = tiff.width();
        tiffheight = tiff.height();
        const gl = this.gl;
        tiff.close();

    }

    console.log(tiffwidth, tiffheight,count);
        // A begin
    var geometry = new THREE.BoxGeometry(tiffwidth * 3, tiffheight * 4, count);

        /**顶点坐标(纹理映射位置)*/
    geometry.translate(tiffwidth * 1.5, tiffheight * 2, count/2);

        console.log(geometry);
        //var p1 = new THREE.Vector3(0 + filestr[0] / 136960 * tiffheight, 0 + filestr[1] / 114560 * tiffwidth, j + filestr[2] / 49280 * count); //顶点1坐标
        //var p2 = new THREE.Vector3(0 + filestr[0] / 136960 * tiffheight, tiffwidth + filestr[1] / 114560 * tiffwidth, j + filestr[2] / 49280 * count); //顶点2坐标
        //var p3 = new THREE.Vector3(tiffheight + filestr[0] / 136960 * tiffheight, tiffwidth + filestr[1] / 114560 * tiffwidth, j + filestr[2] / 49280 * count); //顶点3坐标
        //var p4 = new THREE.Vector3(tiffheight + filestr[0] / 136960 * tiffheight, 0 + filestr[1] / 114560 * tiffwidth, j + filestr[2] / 49280 * count); //顶点4坐标
        //geometry.vertices.push(p1, p2, p3, p4); //顶点坐标添加到geometry对象
        ////console.log(p1, p2, p3, p4);


        ///** 三角面1、三角面2*/
        //var normal = new THREE.Vector3(0, 0, 1); //三角面法向量
        //var face0 = new THREE.Face3(0, 1, 2, normal); //三角面1
        //var face1 = new THREE.Face3(0, 2, 3, normal); //三角面2
        //geometry.faces.push(face0, face1); //三角面1、2添加到几何体
        ///**纹理坐标*/
        //var t0 = new THREE.Vector2(0, 0);//图片左下角
        //var t1 = new THREE.Vector2(1, 0);//图片右下角
        //var t2 = new THREE.Vector2(1, 1);//图片右上角
        //var t3 = new THREE.Vector2(0, 1);//图片左上角
        //uv1 = [t0, t1, t2];//选中图片一个三角区域像素——映射到三角面1
        //uv2 = [t0, t2, t3];//选中图片一个三角区域像素——映射到三角面2
        //geometry.faceVertexUvs[0].push(uv1, uv2);//纹理坐标传递给纹理三角面属性



        ////var texture2 = new THREE.TextureLoader().load( rgba );

        var texture = new THREE.DataTexture3D(rgba, tiffwidth*3, tiffheight*4, count);
        //texture.format = THREE.RedFormat;
        texture.format = THREE.RGBAFormat;
        texture.type = THREE.UnsignedByteType;
        texture.minFilter = texture.magFilter = THREE.LinearFilter;
       // texture.unpackAlignment = 1;
        texture.needsUpdate = true;
        // console.log(texture);
        var shader = THREE.VolumeRenderShader1;

        var uniforms = THREE.UniformsUtils.clone(shader.uniforms);

        volconfig = { clim1: 0, clim2: 1, renderstyle: 'mip', isothreshold: 0.15, colormap: 'gray' };

        uniforms.u_data.value = texture;
        uniforms.u_size.value.set(tiffwidth*3, tiffheight*4, count);
        uniforms.u_clim.value.set(volconfig.clim1, volconfig.clim2);
        uniforms.u_renderstyle.value = volconfig.renderstyle == 'mip' ? 0 : 1; // 0: MIP, 1: ISO
        uniforms.u_renderthreshold.value = volconfig.isothreshold; // For ISO renderstyle
        //uniforms.u_cmdata.value = cmtextures[volconfig.colormap];

        material = new THREE.ShaderMaterial({
            uniforms: uniforms,
            vertexShader: shader.vertexShader,
            fragmentShader: shader.fragmentShader,
            side: THREE.BackSide // The volume shader uses the backface as its "reference point"
        });
    material.transparent = true;
    material.alphaTest = 0.1;
        // Mesh

        var mesh = new THREE.Mesh(geometry, material);
        scene[0].add(mesh);

        render();

       



    

    var sphereGeometry = new THREE.SphereBufferGeometry(0.1, 32, 32);
    var sphereMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000 });
    for (var i = 0; i < 40; i++) {
        var sphere = new THREE.Mesh(sphereGeometry, sphereMaterial);
        scene[0].add(sphere);
        spheres.push(sphere);
    }

    var edges = new THREE.EdgesGeometry(bbox);
    var line = new THREE.LineSegments(edges, new THREE.LineBasicMaterial({ color: 0xffffff }));
    line.position.set(269, 427, 77);
    scene[0].add(line);




    var raycaster = new THREE.Raycaster();
    var mouse = new THREE.Vector2();



    tiff_width = [179, 215, 239, 253, 253];
    tiff_height = [213, 244, 244, 253, 253];
    tiff_depth = [153, 154, 205, 246, 247];
    unit_width = [114560, 68800, 38240, 20240, 10120];
    unit_height = [136960, 78080, 39040, 20240, 10120];
    unit_depth = [98560, 49280, 32800, 19680, 9840];

    async function onMouseClick(event) {

        //通过鼠标点击的位置计算出raycaster所需要的点的位置，以屏幕中心为原点，值的范围为-1到1.

        mouse.x = (event.clientX / width) * 2 - 1;
        mouse.y = - (event.clientY / height) * 2 + 1;
        console.log(width, height, mouse.x, mouse.y)

        // 通过鼠标点的位置和当前相机的矩阵计算出raycaster
        raycaster.setFromCamera(mouse, camera[level]);

        // 获取raycaster直线和所有模型相交的数组集合
        var intersects = raycaster.intersectObjects(scene[level].children);
        var intensity = new Array();
        for (var i = 0; i < intersects.length; i++) {

            intensity[i] = intersects[i].object.material.map.image.data[(parseInt(intersects[i].point.y % tiff_width[level]) - 1) * tiff_height[level] + parseInt(intersects[i].point.x % tiff_height[level])];
            //intensity[i] = intersects[i].object.material.map.image.data[((parseInt(intersects[i].point.y % tiff_width[level])) + (parseInt(intersects[i].point.x % tiff_height[level]) * tiff_width[level])) * 4];
        }
        //console.log(((parseInt(intersects[30].point.y % tiff_width[level])) + (parseInt(intersects[30].point.x % tiff_height[level]) * tiff_width[level])) * 4);
        //console.log((parseInt(intersects[30].point.x % tiff_height[level]) * tiff_width[level])); 
        var indexOfMaxintensity = intensity.reduce((iMax, x, i, arr) => x > arr[iMax] ? i : iMax, 0);
        console.log(indexOfMaxintensity);

        //console.log(parseInt(intersects[0].point.x % 213), parseInt(intersects[0].point.y % 179));
        //console.log((parseInt(intersects[0].point.y % tiff_width[level]) - 1) * tiff_height[level] + parseInt(intersects[0].point.x % tiff_height[level]));
        //console.log(intersects[0].object.material.map.image.data[(parseInt(intersects[0].point.y % tiff_width[level])-1) * tiff_height[level] + parseInt(intersects[0].point.x % tiff_height[level])]);

        zoomin_width = Math.floor(intersects[indexOfMaxintensity].point.x / tiff_height[level] * unit_height[level] / unit_height[level + 1]) * unit_height[level + 1];
        zoomin_height = Math.floor(intersects[indexOfMaxintensity].point.y / tiff_width[level] * unit_width[level] / unit_width[level + 1]) * unit_width[level + 1];
        zoomin_depth = Math.floor(intersects[indexOfMaxintensity].point.z / tiff_depth[level] * unit_depth[level] / unit_depth[level + 1]) * unit_depth[level + 1];
        console.log(level);
        console.log(zoomin_width, zoomin_height, zoomin_depth);

        //while (scene[0].children.length > 0) { scene[0].remove(scene[0].children[0]); }









        const re = await axios.get(`api/tiff/${+id + level + 1}`)


        var files = re.data;

        for (index in files) {
            const file = files[index];
            var fileName = $("#file").val();

            var regex = /\d+/g;
            var filestr = file.match(regex);

            if (parseInt(filestr[0] / unit_height[level + 1]) * unit_height[level + 1] == zoomin_width && parseInt(filestr[1] / unit_width[level + 1]) * unit_width[level + 1] == zoomin_height && parseInt(filestr[2] / unit_depth[level + 1]) * unit_depth[level + 1] == zoomin_depth) {
                //if ((filestr[0] <= zoomin_width + unit_height[level + 1]) && (filestr[0] >= zoomin_width - unit_height[level + 1]) && (filestr[1] <= zoomin_height + unit_width[level + 1]) && (filestr[1] >= zoomin_height - unit_width[level + 1]) && (filestr[2] <= zoomin_depth) && (filestr[2] >= zoomin_depth - unit_depth[level + 1])) {
                // if ((filestr[0] <= zoomin_width ) && (filestr[0] >= zoomin_width - unit_height[level + 1]) && (filestr[1] <= zoomin_height ) && (filestr[1] >= zoomin_height - unit_width[level + 1]) && (filestr[2] <= zoomin_depth) && (filestr[2] >= zoomin_depth - unit_depth[level + 1])) {
                const response = await axios.get(`api/tiff/${+id + level + 1}/${file}`, { responseType: "arraybuffer" })
                console.log(file);


                var tiff = new Tiff({ buffer: response.data });
                var count = tiff.countDirectory();

                var j;
                for (j = 0; j < count; j++) {
                    tiff.setDirectory(j);
                    var buffer = tiff.readRGBAImage();
                    //console.log(tiffwidth,tiffheight);
                    const rgba = new Uint8Array(buffer);

                    var k; var contrast = 12; var thresh = 2;
                    for (k = 0; k < rgba.byteLength; k = k + 4) {
                        if (rgba[k] < thresh) { rgba[k + 3] = 0; }
                        rgba[k] *= contrast;
                        rgba[k + 1] *= contrast;
                        rgba[k + 2] *= contrast;
                        if (rgba[k] == 0 && rgba[k + 1] == 0 && rgba[k + 1] == 0)
                            rgba[k + 3] = 0;

                    }

                    const gl = this.gl;



                    // A begin
                    var geometry = new THREE.BoxGeometry(tiffwidth, tiff.height, count);
                    /**顶点坐标(纹理映射位置)*/
                    geometry.translate(tiff_width[level + 1] / 2 + filestr[1] / unit_width[level + 1] * tiff_width[level + 1], ifds[0].height / 2 - 0.5, ifds.length / 2 - 0.5);

                    var p1 = new THREE.Vector3(0 + filestr[0] / unit_height[level + 1] * tiff_height[level + 1], 0 + filestr[1] / unit_width[level + 1] * tiff_width[level + 1], j + filestr[2] / unit_depth[level + 1] * count); //顶点1坐标
                    var p2 = new THREE.Vector3(0 + filestr[0] / unit_height[level + 1] * tiff_height[level + 1], tiff_width[level + 1] + filestr[1] / unit_width[level + 1] * tiff_width[level + 1], j + filestr[2] / unit_depth[level + 1] * count); //顶点2坐标
                    var p3 = new THREE.Vector3(tiff_height[level + 1] + filestr[0] / unit_height[level + 1] * tiff_height[level + 1], tiff_width[level + 1] + filestr[1] / unit_width[level + 1] * tiff_width[level + 1], j + filestr[2] / unit_depth[level + 1] * count); //顶点3坐标
                    var p4 = new THREE.Vector3(tiff_height[level + 1] + filestr[0] / unit_height[level + 1] * tiff_height[level + 1], 0 + filestr[1] / unit_width[level + 1] * tiff_width[level + 1], j + filestr[2] / unit_depth[level + 1] * count); //顶点4坐标
                    //var p1 = new THREE.Vector3(0, 0, j);
                    //var p2 = new THREE.Vector3(0, tiff_width[level + 1], j);
                    //var p3 = new THREE.Vector3(tiff_height[level + 1], tiff_width[level + 1], j);
                    //var p4 = new THREE.Vector3(tiff_height[level + 1], 0, j);

                    geometry.vertices.push(p1, p2, p3, p4); //顶点坐标添加到geometry对象
                    //console.log(filestr[0], filestr[1]);
                    //console.log(p1, p2, p3, p4);




                    /** 三角面1、三角面2*/
                    var normal = new THREE.Vector3(0, 0, 1); //三角面法向量
                    var face0 = new THREE.Face3(0, 1, 2, normal); //三角面1
                    var face1 = new THREE.Face3(0, 2, 3, normal); //三角面2
                    geometry.faces.push(face0, face1); //三角面1、2添加到几何体
                    /**纹理坐标*/
                    var t0 = new THREE.Vector2(0, 0);//图片左下角
                    var t1 = new THREE.Vector2(1, 0);//图片右下角
                    var t2 = new THREE.Vector2(1, 1);//图片右上角
                    var t3 = new THREE.Vector2(0, 1);//图片左上角
                    uv1 = [t0, t1, t2];//选中图片一个三角区域像素——映射到三角面1
                    uv2 = [t0, t2, t3];//选中图片一个三角区域像素——映射到三角面2
                    geometry.faceVertexUvs[0].push(uv1, uv2);//纹理坐标传递给纹理三角面属性



                    //var texture2 = new THREE.TextureLoader().load( rgba );

                    //console.log(texture2);
                    var texture = new THREE.DataTexture(rgba, tiffwidth*3, tiffheight*4, THREE.RGBAFormat);

                    texture.needsUpdate = true;
                    //console.log(texture);
                    var material = new THREE.MeshBasicMaterial({ map: texture, side: THREE.BackSide });
                    material.transparent = true;
                    //material.blending = THREE[ "MultiplyBlending"];
                    //material.depthWrite = false,
                    //material.depthTest = false,
                    material.blending = THREE["CustomBlending"];
                    material.blendEquation = THREE.MaxEquation;
                    material.alphaTest = 0.5;

                    var mesh = new THREE.Mesh(geometry, material);
                    //mesh.position.set(128, 128, j);
                    scene[level + 1].add(mesh);


                }
                tiff.close();
                //renderscene2();

            }
        }



        var sphereGeometry = new THREE.SphereBufferGeometry(0.1, 32, 32);
        var sphereMaterial = new THREE.MeshBasicMaterial({ color: 0xff0000 });
        for (var i = 0; i < 40; i++) {
            var sphere = new THREE.Mesh(sphereGeometry, sphereMaterial);
            scene[level + 1].add(sphere);
            spheres.push(sphere);
        }

        bbox2 = new THREE.BoxGeometry(tiff_height[level + 1], tiff_width[level + 1], tiff_depth[level + 1]);
        var edges = new THREE.EdgesGeometry(bbox2);
        var line = new THREE.LineSegments(edges, new THREE.LineBasicMaterial({ color: 0xffffff }));
        line.position.set(tiff_height[level + 1] / 2 + p1.x, tiff_width[level + 1] / 2 + p1.y, p1.z - tiff_depth[level + 1] / 2);
        scene[level + 1].add(line);
        //console.log(line.position);
        controls = new THREE.OrbitControls(camera[level + 1], document.getElementById('view'));//创建控件对象
        controls.addEventListener('change', render);//监听鼠标、键盘事件
        controls.target.copy(line.position);



        var axesHelper = new THREE.AxesHelper(300);
        axesHelper.position.set(p1.x, p1.y, p1.z - tiff_depth[level + 1]);
        //console.log(axesHelper.position);
        scene[level + 1].add(axesHelper);


        camera[level + 1].position.x = line.position.x;

        //camera.position.x = 146;
        camera[level + 1].position.y = line.position.y + 30;
        camera[level + 1].position.z = line.position.z + 450;

        //camera.position.addVectors(p1, p3);
        //camera.position.divideScalar(2);
        camera[level + 1].lookAt(line.position);
        //console.log(camera.position);


        level = level + 1;
        render();

    }


    // 添加window 的resize事件监听
    window.addEventListener('resize', onWindowResize);
    function onWindowResize() {
        width = document.getElementById('view').clientWidth;
        height = document.getElementById('view').clientHeight;
        // 重新设置相机宽高比例
        camera[level].aspect = width / height;
        // 更新相机投影矩阵
        camera[level].updateProjectionMatrix();
        // 重新设置渲染器渲染范围
        renderer.setSize(width, height);
        setTimeout(renderer.render(scene[level], camera[level]), 300);
    }



    window.addEventListener('dblclick', onMouseClick, false);
    // window.addEventListener('resize', onWindowResize, false);

    document.getElementById('left-content').addEventListener('fullscreenchange', render);




    // handle success
    // this.imgs.push(new Tiff({ buffer: response.data }))



}

function mergeTypedArraysUnsafe(a, b) {
    var c = new a.constructor(a.length + b.length);
    console.log(c);
    c.set(a);
    c.set(b, a.length);

    return c;
}

function VF(event) {
    width = document.getElementById('view').clientWidth;
    height = document.getElementById('view').clientHeight;
    event.preventDefault();
    mouse.x = (event.clientX / width) * 2 - 1;
    mouse.y = - (event.clientY / height) * 2 + 1;

    controls.removeEventListener('change', render);
    points = new Array();
    var raycaster = new THREE.Raycaster();

    raycaster.setFromCamera(mouse, camera);
    var intersects = raycaster.intersectObjects(scene.children);
    var intensity = new Array();
    for (var i = 0; i < intersects.length; i++) {

        intensity[i] = intersects[i].object.material.map.image.data[(parseInt(intersects[i].point.y % tiff_width[level]) - 1) * tiff_height[level] + parseInt(intersects[i].point.x % tiff_height[level])];

    }
    //console.log(intensity);
    var indexOfMaxintensity = intensity.reduce((iMax, x, i, arr) => x > arr[iMax] ? i : iMax, 0);
    //console.log(intersects[indexOfMaxintensity].point);
    //console.log(intersects[0].point);


    if (intersects !== null) {
        spheres[spheresIndex].position.copy(intersects[indexOfMaxintensity].point);
        spheres[spheresIndex].scale.set(1, 1, 1);
        spheresIndex = (spheresIndex + 1) % spheres.length;

    }
    //for (var i = 0; i < spheres.length; i++) {
    //    var sphere = spheres[i];
    //    //sphere.scale.multiplyScalar(0.98);
    //    //sphere.scale.clampScalar(0.01, 1);
    //}

    render();
    //console.log(spheres);





}
function onkeydown(event) {
    switch (event.keyCode) {
        case 68: draw(); break;
        case 27: document.removeEventListener('mousemove', VF, false); controls.addEventListener('change', render); break;
        case 66: goback(); break;



    }
}

window.addEventListener('keydown', onkeydown, false);
function draw() {
    //window.addEventListener('dblclick', window.addEventListener('mousemove', VF, false), false);
    document.addEventListener('mousemove', VF, false);


    //window.addEventListener('mouseup', quitVF, false);
    //function VF(event) {

    //controls.removeEventListener('change', render);
    //points = new Array();
    //var raycaster = new THREE.Raycaster();

    //raycaster.setFromCamera(mouse, camera);
    //var intersects = raycaster.intersectObjects(scene.children);
    //console.log(intersects[0].point);


    //}

    //function quitVF(event) {
    //    window.removeEventListener('mousedown', window.addEventListener('mousemove', VF, false), false);
    //    window.removeEventListener('mousemove', VF, false);
    //    controls.addEventListener('change', render);




    //}

}

function goback() {
    while (scene[level].children.length > 0) { scene[level].remove(scene[level].children[0]); }
    level = level - 1;
    render();


}