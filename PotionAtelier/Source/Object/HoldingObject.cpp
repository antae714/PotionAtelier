#include "HoldingObject.h"
#include "../Components/Holding.h"

void HoldingObject::Awake()
{
	AddComponent<Holding>();

	//auto& tr = AddComponent<TextRender>();
	


	//CubeObject::Awake();
	//auto& aa = GetComponent<CubeMeshRender>();
	//aa.materialAsset.GetAssetData()->customData.SetField("WTF", Color{ 1, 0, 0, 1 });

	/*auto& com = AddComponent<TextRender>();
	com.SetText(L"���ع�����λ��̸�����⵵��\n����������"); <- ??? ������ �̰�
	com.SetSize(30);
	com.SetColor({ 1, 0, 0, 1 });
	com.SetPosition({ 900, 600 });
	com.SetType(FontType::Ulsan);*/

	/*auto& ui = AddComponent<UIRenderComponenet>();
	ui.SetTransform(300, 300, 300, 300);
	ui.SetTexture(L"./Resource/Sample/Item_Flower.png");

	auto& ev = AddComponent<EventListener>();
	ev.SetOnClickFunction([]()
	{
		printf("I'm clicked!\n");
	});*/
}
