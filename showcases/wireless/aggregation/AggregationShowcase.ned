package inet.showcases.wireless.aggregation;

import inet.networklayer.configurator.ipv4.Ipv4NetworkConfigurator;
import inet.node.inet.AdhocHost;
import inet.physicallayer.common.packetlevel.NoiseSource;
import inet.physicallayer.ieee80211.packetlevel.Ieee80211DimensionalRadioMedium;
import inet.visualizer.integrated.IntegratedVisualizer;


network AggregationShowcase
{
    @display("bgb=675,400");
    submodules:
        configurator: Ipv4NetworkConfigurator {
            @display("p=50,50");
        }
        radioMedium: Ieee80211DimensionalRadioMedium {
            @display("p=50,100");
        }
        visualizer: IntegratedVisualizer {
            @display("p=50,150");
        }
        host1: AdhocHost {
            @display("p=288,110;i=misc/smartphone");
        }
        host2: AdhocHost {
            @display("p=480,258;i=misc/smartphone");
        }
}

network AggregationShowcase2 extends AggregationShowcase
{
    submodules:
        noiseSource: NoiseSource {
            @display("p=157,86");
        }
}
