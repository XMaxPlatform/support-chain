


function init(code,type)
{
	StoreRecord(StrToName("defaultscope"),StrToName("testdata"),StrToName("testacc"),999);
}

function apply(code ,type)
{
	if(code == StrToName("testcontract"))
	{
		if(type == StrToName("transfer"))
		{
			transfer(code,type);
		}
	}
}

function transfer()
{
	var amount = GetMsgData(code,type,"account","i64")
	
	StoreRecord(StrToName("defaultscope"),StrToName("testdata"),StrToName("testdata"),amount);
}