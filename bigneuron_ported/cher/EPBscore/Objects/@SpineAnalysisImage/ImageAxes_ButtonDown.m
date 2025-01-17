function obj = ImageAxes_ButtonDownFcn(obj)
global self;
global ogh;
ogh=obj.gh;
fig=obj.gh.imageGUI.Figure;
currentpoint=ceil(get(gca,'CurrentPoint'));
type = get(gcbf,'SelectionType');
handles=guidata(fig);

switch obj.state.display.mousemode
    case 'editspine'
        switch type
            case 'normal'   % left button Click
                if (obj.state.display.displayfiltered) & isfield(obj.data,'filteredArray')
                    obj.state.display.positionintensity=obj.data.filteredArray(currentpoint(1,2),currentpoint(1,1),obj.state.display.currentz);
                    obj.state.display.positionx=currentpoint(1,1);
                    obj.state.display.positiony=currentpoint(1,2);
                    self=struct(obj);
                    updateGUIbyglobal('self.state.display.positionx');
                    updateGUIbyglobal('self.state.display.positiony');
                    updateGUIbyglobal('self.state.display.positionintensity');
                elseif (obj.state.display.displaybinary) & isfield(obj.data,'filteredArray')
                    obj.state.display.positionintensity=obj.data.filteredArray(currentpoint(1,2),currentpoint(1,1),obj.state.display.currentz);
                    obj.state.display.positionx=currentpoint(1,1);
                    obj.state.display.positiony=currentpoint(1,2);
                    self=struct(obj);
                    updateGUIbyglobal('self.state.display.positionx');
                    updateGUIbyglobal('self.state.display.positiony');
                    updateGUIbyglobal('self.state.display.positionintensity');
                elseif (obj.state.display.displayraw)
                    obj.state.display.positionintensity=obj.data.imageArray(currentpoint(1,2),currentpoint(1,1),obj.state.display.currentz);
                    obj.state.display.positionx=currentpoint(1,1);
                    obj.state.display.positiony=currentpoint(1,2);
                    self=struct(obj);
                    updateGUIbyglobal('self.state.display.positionx');
                    updateGUIbyglobal('self.state.display.positiony');
                    updateGUIbyglobal('self.state.display.positionintensity');
                end
            case 'extend'   % shift click to add
                if (obj.state.display.displayspines)
                    
                    j=size(obj.data.spines,2)+1;
                    obj.data.spines(j).type=0;
                    obj.data.spines(j).color=[0.7,0.7,0.7];
                    obj.data.spines(j).voxels=3;
                    obj.data.spines(j).voxel=[0 0 0;currentpoint(1,2)-1 currentpoint(1,1)-1 obj.state.display.currentz-1;currentpoint(1,2)-1 currentpoint(1,1)-1 obj.state.display.currentz-1]';
                    
                    distances=[];
                    den_ind=obj.state.display.axonnumber;
                    if (den_ind==0)   % if not specified, find the closest neurite             
                        for i=1:size(obj.data.dendrites,2)
                            dendrite=double(obj.data.dendrites(i).voxel);
                            spine=double(obj.data.spines(j).voxel);
                            dis=(dendrite(1,:)-spine(1,2)).*(dendrite(1,:)-spine(1,2))+(dendrite(2,:)-spine(2,2)).*(dendrite(2,:)-spine(2,2))+(dendrite(3,:)-spine(3,2)).*(dendrite(3,:)-spine(3,2));
                            [a,ind]=min(dis);
                            distances=[distances a];
                        end
                        [a,den_ind]=min(distances);
                    end
                    
                    %Calculate distances
                    dendrite=double(obj.data.dendrites(den_ind).voxel);
                    spine=double(obj.data.spines(j).voxel);
                    dis=sqrt((dendrite(1,:)-spine(1,2)).*(dendrite(1,:)-spine(1,2))+(dendrite(2,:)-spine(2,2)).*(dendrite(2,:)-spine(2,2))+(dendrite(3,:)-spine(3,2)).*(dendrite(3,:)-spine(3,2)));
                    [a,ind]=min(dis);
                    dendrite=double(obj.data.dendrites(den_ind).voxel);
                    obj.data.spines(j).den_ind=den_ind;
                    if (a<5)
                        [maxval,maxind]=max(double(obj.data.dendrites(den_ind).maxint(ind-5:ind+5)));
                        ind=ind+maxind-6;
                        obj.data.spines(j).dendis=ind;
                        
                        obj.data.spines(j).voxel(:,1)=uint16(dendrite(1:3,ind));
                        
                        obj.data.spines(j).dendis=ind;
                        obj.data.spines(j).len=0;
                        obj.data.spines(j).edittype=0;
                        obj.data.spines(j).label=max([obj.data.spines.label])+1;
                        obj.data.spines(j).intensity=obj.data.dendrites(den_ind).maxintcorrected(ind);
                    else
                        left=max(spine(2,2)-5,1);
                        right=min(spine(2,2)+5,size(obj.data.filteredArray,1));
                        up=max(spine(1,2)-5,1);
                        down=min(spine(1,2)+5,size(obj.data.filteredArray,2));
                        top=max(spine(3,2)-5,1);
                        bottom=min(spine(3,2)+5,size(obj.data.filteredArray,3)); 
                        [maxval,maxindz]=max(obj.data.filteredArray(up:down,left:right,top:bottom),[],3);
                        [maxval,maxindy]=max(maxval);
                        [maxval,maxindx]=max(maxval);
                        maxindy=maxindy(maxindx);
                        maxindz=maxindz(maxindx,maxindy);
                        obj.data.spines(j).voxel=[0 0 0;up+maxindy-1 left+maxindx-1 top+maxindz-1;up+maxindy-1 left+maxindx-1 top+maxindz-1]';
                        obj.data.spines(j).voxel(:,1)=uint16(dendrite(1:3,ind));
                        spine=double(obj.data.spines(j).voxel);
                        obj.parameters.xspacing=0.083;
                        obj.parameters.yspacing=0.083;
                        obj.parameters.zspacing=1;
                        xs= obj.parameters.xspacing;
                        ys=obj.parameters.yspacing;
                        zs=obj.parameters.zspacing;
                        dis=sqrt((dendrite(1,:)-spine(1,2)).*(dendrite(1,:)-spine(1,2))*ys*ys+(dendrite(2,:)-spine(2,2)).*(dendrite(2,:)-spine(2,2))*xs*xs+(dendrite(3,:)-spine(3,2)).*(dendrite(3,:)-spine(3,2))*zs*zs);
                        [a,ind]=min(dis);
                                            
                        obj.data.spines(j).dendis=ind;
                        obj.data.spines(j).len=a;
                        obj.data.spines(j).edittype=0;
                        obj.data.spines(j).label=max([obj.data.spines.label])+1;
                        obj.data.spines(j).intensity=(double(obj.data.filteredArray(spine(1,1),spine(2,1),spine(3,1)))-obj.data.imagemedian)/obj.data.dendrites(den_ind).medianfilteredsingle(ind);                        
                    end
                    
                    %         den=data.dendrites(data.spines(j).den_ind).voxel;
                    %         dend=den(:,max(1,data.spines(j).dendis-5):min(size(den,2),data.spines(j).dendis+5));
                    %         coeff=(double([ones(1,size(dend,2)); dend(1,:)]))'\(double(dend(2,:)))';
                    %         obj.data.spines(j).side=sign(double(data.spines(j).voxel(2,2))-co
                    %         eff(2)*double(data.spines(j).voxel(1,2))-coef
                    %         f(1));
                    obj.data.slicespines={};            % Take care of slice spines for display 
                    for i=1:size(obj.data.filteredArray,3)
                        ind=1;
                        %   obj.data.slicespines(i)=struct([]);
                        for j=1:size(obj.data.spines,2)
                            pos=find(obj.data.spines(j).voxel(3,:)==i-1);
                            if (pos)
                                obj.data.slicespines(i).spines(ind)=struct('voxel',[obj.data.spines(j).voxel(1,pos);obj.data.spines(j).voxel(2,pos)],'index',j);
                                ind=ind+1;    
                            end
                        end
                    end;
                end     
            case 'alt'    % click right button to delete
                if (obj.state.display.displayspines)
                    distance=[];
                    for j=1:size(obj.data.spines,2)
                        if obj.data.spines(j).voxel(3,2)==obj.state.display.currentz-1
                            distance=[distance sqrt((obj.data.spines(j).voxel(1,2)-currentpoint(1,2)+1).*(obj.data.spines(j).voxel(1,2)-currentpoint(1,2)+1)+(obj.data.spines(j).voxel(2,2)-currentpoint(1,1)+1).*(obj.data.spines(j).voxel(2,2)-currentpoint(1,1)+1))];
                        else
                            distance=[distance 100000000];
                        end
                    end
                    [a,ind]=min(distance);
                    if (a<10000000)
                        obj.data.spines(ind).edittype=1; %edittype of 1 is deleted
                    end
                end
%                  case 'editmarks'
%                   switch type
%                    case 'normal'   % left button Click
%                     if (obj.state.display.displayfiltered)
%                         obj.state.display.positionintensity=data.filteredArray(currentpoint(1,2)-1,currentpoint(1,1)-1,obj.state.display.currentz);
%                         updateGUIbyField(handles,data,'positionintensity');
%                         obj.state.display.positionx=currentpoint(1,1)-1;
%                         obj.state.display.positiony=currentpoint(1,2)-1;
%                         updateGUIbyField(handles,data,'currentpos');
%                     elseif (obj.state.display.displayraw)
%                         obj.state.display.positionintensity=data.imageArray(currentpoint(1,2)-1,currentpoint(1,1)-1,obj.state.display.currentz);
%                         updateGUIbyField(handles,data,'positionintensity');
%                         obj.state.display.positionx=currentpoint(1,1)-1;
%                         obj.state.display.positiony=currentpoint(1,2)-1;
%                         updateGUIbyField(handles,data,'currentpos');
%                     end
%                    case 'extend'   % click both button to add
%                        obj.state.display.marks=[obj.state.display.marks;currentpoint(1,1)-1 currentpoint(1,2)-1 obj.state.display.currentz];
%                        displayImage(handles,data);
%                    case 'alt'    % click right button to delete
        end 
end
