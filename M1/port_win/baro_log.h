struct BarometricData
{
	uint32_t tick;
	float temperature;
	float pressure;
	
} baro_data[] = {
	{ 0x00050353, 27.530001, 100275.601562 },
	{ 0x0005037B, 27.520000, 100273.062500 },
	{ 0x000503A3, 27.530001, 100274.343750 },
	{ 0x000503CB, 27.530001, 100273.250000 },
	{ 0x000503F3, 27.520000, 100272.484375 },
	{ 0x0005041B, 27.520000, 100273.023438 },
	{ 0x00050443, 27.520000, 100275.765625 },
	{ 0x0005046B, 27.530001, 100273.250000 },
	{ 0x00050493, 27.520000, 100274.468750 },
	{ 0x000504BB, 27.520000, 100273.921875 },
	{ 0x000504E3, 27.530001, 100273.796875 },
	{ 0x0005050B, 27.520000, 100275.406250 },
	{ 0x00050533, 27.520000, 100273.750000 },
	{ 0x0005055B, 27.520000, 100273.210938 },
	{ 0x00050583, 27.530001, 100270.718750 },
	{ 0x000505AB, 27.520000, 100272.703125 },
	{ 0x000505D3, 27.510000, 100270.656250 },
	{ 0x000505FB, 27.510000, 100274.281250 },
	{ 0x00050623, 27.520000, 100270.859375 },
	{ 0x0005064B, 27.510000, 100271.562500 },
	{ 0x00050673, 27.520000, 100271.437500 },
	{ 0x0005069B, 27.510000, 100268.687500 },
	{ 0x000506C3, 27.510000, 100269.953125 },
	{ 0x000506EB, 27.520000, 100270.328125 },
	{ 0x00050713, 27.510000, 100269.953125 },
	{ 0x0005073B, 27.510000, 100268.140625 },
	{ 0x00050763, 27.500000, 100267.218750 },
	{ 0x0005078B, 27.510000, 100266.328125 },
	{ 0x000507B3, 27.520000, 100266.171875 },
	{ 0x000507DB, 27.500000, 100264.156250 },
	{ 0x00050803, 27.510000, 100266.156250 },
	{ 0x0005082B, 27.520000, 100266.890625 },
	{ 0x00050853, 27.510000, 100264.531250 },
	{ 0x0005087B, 27.500000, 100260.562500 },
	{ 0x000508A3, 27.510000, 100262.718750 },
	{ 0x000508CB, 27.510000, 100263.265625 },
	{ 0x000508F3, 27.500000, 100262.359375 },
	{ 0x0005091B, 27.510000, 100262.187500 },
	{ 0x00050943, 27.510000, 100262.000000 },
	{ 0x0005096B, 27.500000, 100261.453125 },
	{ 0x00050993, 27.510000, 100263.796875 },
	{ 0x000509BB, 27.510000, 100263.804688 },
	{ 0x000509E3, 27.520000, 100265.085938 },
	{ 0x00050A0B, 27.500000, 100262.171875 },
	{ 0x00050A33, 27.510000, 100263.437500 },
	{ 0x00050A5B, 27.510000, 100260.375000 },
	{ 0x00050A83, 27.510000, 100262.734375 },
	{ 0x00050AAB, 27.510000, 100262.000000 },
	{ 0x00050AD3, 27.510000, 100261.656250 },
	{ 0x00050AFB, 27.520000, 100262.390625 },
	{ 0x00050B23, 27.510000, 100260.554688 },
	{ 0x00050B4B, 27.510000, 100258.562500 },
	{ 0x00050B73, 27.510000, 100261.648438 },
	{ 0x00050B9B, 27.510000, 100260.203125 },
	{ 0x00050BC3, 27.520000, 100259.312500 },
	{ 0x00050BEB, 27.520000, 100260.250000 },
	{ 0x00050C13, 27.520000, 100259.859375 },
	{ 0x00050C3B, 27.510000, 100260.382812 },
	{ 0x00050C63, 27.510000, 100259.468750 },
	{ 0x00050C8B, 27.510000, 100259.843750 },
	{ 0x00050CB3, 27.510000, 100262.007812 },
	{ 0x00050CDB, 27.510000, 100258.765625 },
	{ 0x00050D03, 27.510000, 100260.929688 },
	{ 0x00050D2B, 27.510000, 100261.109375 },
	{ 0x00050D53, 27.510000, 100259.289062 },
	{ 0x00050D7B, 27.510000, 100259.843750 },
	{ 0x00050DA3, 27.520000, 100260.757812 },
	{ 0x00050DCB, 27.520000, 100260.937500 },
	{ 0x00050DF3, 27.500000, 100257.843750 },
	{ 0x00050E1B, 27.510000, 100258.578125 },
	{ 0x00050E43, 27.510000, 100260.750000 },
	{ 0x00050E6B, 27.510000, 100262.546875 },
	{ 0x00050E93, 27.520000, 100263.093750 },
	{ 0x00050EBB, 27.510000, 100262.718750 },
	{ 0x00050EE3, 27.510000, 100263.812500 },
	{ 0x00050F0B, 27.510000, 100264.718750 },
	{ 0x00050F33, 27.510000, 100264.000000 },
	{ 0x00050F5B, 27.510000, 100266.875000 },
	{ 0x00050F83, 27.520000, 100267.437500 },
	{ 0x00050FAB, 27.520000, 100268.343750 },
	{ 0x00050FD3, 27.510000, 100269.046875 },
	{ 0x00050FFB, 27.510000, 100266.890625 },
	{ 0x00051023, 27.510000, 100266.156250 },
	{ 0x0005104B, 27.510000, 100265.968750 },
	{ 0x00051073, 27.510000, 100267.773438 },
	{ 0x0005109B, 27.510000, 100268.679688 },
	{ 0x000510C3, 27.510000, 100265.609375 },
	{ 0x000510EB, 27.520000, 100267.968750 },
	{ 0x00051113, 27.510000, 100268.312500 },
	{ 0x0005113B, 27.520000, 100269.593750 },
	{ 0x00051163, 27.510000, 100266.335938 },
	{ 0x0005118B, 27.510000, 100266.156250 },
	{ 0x000511B3, 27.510000, 100265.781250 },
	{ 0x000511DB, 27.510000, 100267.593750 },
	{ 0x00051203, 27.510000, 100268.312500 },
	{ 0x0005122B, 27.500000, 100268.140625 },
	{ 0x00051253, 27.500000, 100267.234375 },
	{ 0x0005127B, 27.500000, 100269.023438 },
	{ 0x000512A3, 27.500000, 100271.031250 },
	{ 0x000512CB, 27.500000, 100271.187500 },
	{ 0x000512F3, 27.490000, 100270.476562 },
	{ 0x0005131B, 27.490000, 100269.203125 },
	{ 0x00051343, 27.490000, 100271.742188 },
	{ 0x0005136B, 27.490000, 100273.367188 },
	{ 0x00051393, 27.490000, 100271.000000 },
	{ 0x000513BB, 27.480000, 100270.820312 },
	{ 0x000513E3, 27.490000, 100273.171875 },
	{ 0x0005140B, 27.490000, 100271.359375 },
	{ 0x00051433, 27.490000, 100273.546875 },
	{ 0x0005145B, 27.490000, 100273.718750 },
	{ 0x00051483, 27.490000, 100273.359375 },
	{ 0x000514AB, 27.490000, 100271.726562 },
	{ 0x000514D3, 27.490000, 100271.562500 },
	{ 0x000514FB, 27.490000, 100275.695312 },
	{ 0x00051523, 27.490000, 100272.093750 },
	{ 0x0005154B, 27.490000, 100273.187500 },
	{ 0x00051573, 27.490000, 100272.281250 },
	{ 0x0005159B, 27.490000, 100271.187500 },
	{ 0x000515C3, 27.490000, 100273.000000 },
	{ 0x000515EB, 27.500000, 100273.375000 },
	{ 0x00051613, 27.490000, 100273.171875 },
	{ 0x0005163B, 27.500000, 100274.453125 },
	{ 0x00051663, 27.490000, 100273.187500 },
	{ 0x0005168B, 27.490000, 100272.265625 },
	{ 0x000516B3, 27.490000, 100271.375000 },
	{ 0x000516DB, 27.490000, 100273.187500 },
	{ 0x00051703, 27.490000, 100272.992188 },
	{ 0x0005172B, 27.480000, 100271.718750 },
	{ 0x00051753, 27.490000, 100274.078125 },
	{ 0x0005177B, 27.490000, 100274.968750 },
	{ 0x000517A3, 27.490000, 100275.156250 },
	{ 0x000517CB, 27.490000, 100277.335938 },
	{ 0x000517F3, 27.490000, 100273.718750 },
	{ 0x0005181B, 27.500000, 100276.796875 },
	{ 0x00051843, 27.490000, 100276.968750 },
	{ 0x0005186B, 27.490000, 100276.421875 },
	{ 0x00051893, 27.490000, 100276.062500 },
	{ 0x000518BB, 27.490000, 100278.421875 },
	{ 0x000518E3, 27.490000, 100277.867188 },
	{ 0x0005190B, 27.490000, 100279.312500 },
	{ 0x00051933, 27.490000, 100276.054688 },
	{ 0x0005195B, 27.490000, 100275.515625 },
	{ 0x00051983, 27.490000, 100276.968750 },
	{ 0x000519AB, 27.490000, 100275.515625 },
	{ 0x000519D3, 27.480000, 100273.156250 },
	{ 0x000519FB, 27.480000, 100274.250000 },
	{ 0x00051A23, 27.480000, 100271.718750 },
	{ 0x00051A4B, 27.490000, 100275.156250 },
	{ 0x00051A73, 27.480000, 100277.304688 },
	{ 0x00051A9B, 27.480000, 100274.062500 },
	{ 0x00051AC3, 27.490000, 100274.257812 },
	{ 0x00051AEB, 27.490000, 100275.695312 },
	{ 0x00051B13, 27.480000, 100274.234375 },
	{ 0x00051B3B, 27.480000, 100276.234375 },
};